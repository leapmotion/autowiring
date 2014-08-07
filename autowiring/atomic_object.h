// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include MUTEX_HEADER

template<class object, class lock> class shared_object;
template<class object, class lock> class unlock_object;

///<summary>
///This class provides a standard thread-safe wrapper for data structures.
///Because instantiation may be separate from initialization, this class
///also tracks the initialization status.
///</summary>
///<remarks>
///This class supports two modes of use:
/// - Atomic assignments which can be used to create a mutable local copy,
///   or to modify the contents of shared_object.
/// - Held locks, via unlock_object, which will block all other interactions.
///</remarks>
template<class object, class lock = std::mutex>
class atomic_object {
  friend class shared_object<object, lock>;
  friend class unlock_object<object, lock>;

protected:
  //CONTRACT: If !m_initialized then m_object == object();
  mutable lock m_lock;
  object m_object;
  bool m_initialized;

public:
  ///<summary>
  ///Default constructor yielding initialized() == false.
  ///</summary>
  atomic_object():
  m_initialized(false) {};

  ///<summary>
  ///Initialization yielding initialized() == source.initialized().
  ///</summary>
  ///<remarks>
  ///Intermediate copies can be avoided during construction using:
  /// atomic_object<object> target(*unlock_object<object>(source));
  ///</remarks>
  atomic_object(const atomic_object<object>& source) {
    std::lock_guard<lock> lock_source(source.m_lock);
    m_object = source.m_object;
    m_initialized = source.m_initialized;
  }

  ///<summary>
  ///Initialization yielding initialized() == true.
  ///</summary>
  atomic_object(const object& source):
  m_object(source),
  m_initialized(true) {}

  ///<summary>
  ///Assignment yielding initialized() == source.initialized().
  ///</summary>
  ///<remarks>
  ///This method avoids deadlocks due to self-assignment, and intermediate
  ///copies due to implicit casting.
  ///</remarks>
  atomic_object<object, lock>& operator = (const atomic_object<object>& source) {
    if (this == &source)
      return *this;
    //IMPORTANT: Aquisition of both locks must be atomic.
    //The following code:
    //  m_initialized = source.initialized(m_object);
    //could deadlock with its counterpart in source.
    std::lock(m_lock, source.m_lock);
    m_object = source.m_object;
    m_initialized = source.m_initialized;
    m_lock.unlock();
    source.m_lock.unlock();
    return *this;
  }

  ///<summary>
  ///Assignment yielding initialized() == true.
  ///</summary>
  atomic_object<object, lock>& operator = (const object& source) {
    std::lock_guard<lock> lock_this(m_lock);
    m_initialized = true;
    m_object = source;
    return *this;
  }

  ///<summary>
  ///Cast by copying current state
  ///</summary>
  operator object() const {
    std::lock_guard<lock> lock_this(m_lock);
    return m_object;
  }

  ///<summary>
  ///Reset using default constructor yielding initialized() == false.
  ///</summary>
  ///<return>True if the object was not assigned default values</return>
  bool reset() {
    std::lock_guard<lock> lock_this(m_lock);
    bool was_initialized = m_initialized;
    m_initialized = false;
    m_object = object();
    return was_initialized;
  }

  ///<summary>
  ///Atomic copy of target to this object, only if initialized() == false.
  ///</summary>
  ///<return>True if the object was not assigned default values</return>
  bool reset(const object& target) {
    std::lock_guard<lock> lock_this(m_lock);
    bool was_initialized = m_initialized;
    if (!m_initialized)
      m_object = target;
    m_initialized = true;
    return was_initialized;
  }

  ///<return>True if the object was not assigned default values</return>
  bool initialized() const {
    std::lock_guard<lock> lock_this(m_lock);
    return m_initialized;
  }

  ///<summary>
  ///Atomic copy of this object to target, only if initialized() == true.
  ///</summary>
  ///<return>True if the object was not assigned default values</return>
  bool initialized(object& target) const {
    std::lock_guard<lock> lock_this(m_lock);
    if (m_initialized)
      target = m_object;
    return m_initialized;
  }

  ///<summary>
  ///If uninitialized uses target for initialization.
  ///If initialized assigns current value to target.
  ///</summary>
  ///<returns> Returns +1 for transfer from target to this, -1 for transfer from this to target</returns>
  int transfer(object& target) {
    std::lock_guard<lock> lock_this(m_lock);
    int val = 0;
    if (m_initialized) {
      target = m_object;
      val = +1;
    } else {
      m_object = target;
      m_initialized = true;
      val = -1;
    }
    return val;
  }

  ///<summary>
  ///If neither this nor target are uninitialized, no transfer occurs.
  ///If this is uninitialized and target is not, then this is initialized by target.
  ///If target is uninitialized and this is, then target is initialized by this.
  ///If both this and target are initialized, no transfer occurs.
  ///</summary>
  ///<returns> Returns +1 for transfer from target to this, -1 for transfer from this to target, else 0</returns>
  int transfer(atomic_object<object, lock>& target) {
    std::lock(m_lock, target.m_lock);
    int val = 0;
    if (m_initialized && !target.m_initialized) {
      target.m_object = m_object;
      target.m_initialized = true;
      val = -1;
    }
    if (!m_initialized && target.m_initialized) {
      m_object = target.m_object;
      m_initialized = true;
      val  = +1;
    }
    m_lock.unlock();
    target.m_lock.unlock();
    return val;
  }
};
