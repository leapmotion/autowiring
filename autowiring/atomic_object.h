// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once
#include MUTEX_HEADER

#include "unlock_object.h"
template<class object, class lock> class unlock_object;

///<summary>
///This class provides a standard thread-safe wrapper for data structures.
///</summary>
///<remarks>
///This class supports two modes of use:
/// - Atomic assignments which can be used to create a mutable local copy,
///   or to modify the contents of atomic_object.
/// - Held locks, via child classes that manipulate the protected lock member.
///</remarks>
template<class object_type, class lock_type = std::mutex>
class atomic_object {
public:
  friend class unlock_object<object_type, lock_type>;
  typedef object_type object;
  typedef lock_type lock;
  typedef unlock_object<object, lock> unlock;
  typedef std::shared_ptr<atomic_object<object, lock>> shared;

protected:
  //CONTRACT: If !m_initialized then m_object == object();
  mutable lock m_lock;
  object m_object;

public:
  ///<summary>
  ///Default constructor yielding initialized() == false.
  ///</summary>
  atomic_object() {};

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
  }

  ///<summary>
  ///Initialization yielding initialized() == true.
  ///</summary>
  atomic_object(const object& source):
    m_object(source)
  {}

  ///<summary>
  ///Assignment yielding initialized() == source.initialized().
  ///</summary>
  ///<remarks>
  ///This method avoids deadlocks due to self-assignment,
  ///and intermediate copies due to implicit casting.
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
    m_lock.unlock();
    source.m_lock.unlock();
    return *this;
  }

  ///<summary>
  ///Assignment yielding initialized() == true.
  ///</summary>
  atomic_object<object, lock>& operator = (const object& source) {
    std::lock_guard<lock> lock_this(m_lock);
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
};
