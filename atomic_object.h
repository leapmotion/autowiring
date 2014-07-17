// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "stdafx.h"
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
    std::lock_guard<lock> lock_this(source.m_lock);
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
    std::lock_guard<lock> lock_this(m_lock);
    std::lock_guard<lock> locksource(source.m_lock);
    m_object = source.m_object;
    m_initialized = source.m_initialized;
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

  ///<return>True if the object was not assigned default values</return>
  bool initialized() const {
    std::lock_guard<lock> lock_this(m_lock);
    return m_initialized;
  }

  ///<summary>
  ///Atomic copy of this location to argument location, only if this has location.
  ///</summary>
  ///<return>True if the object was not assigned default values</return>
  bool initialized(object& target) {
    std::lock_guard<lock> lock_this(m_lock);
    if (m_initialized)
      target = m_object;
    return m_initialized;
  }

  ///<summary>
  ///Reset using default constructor yielding initialized() == false.
  ///</summary>
  void reset() {
    std::lock_guard<lock> lock_this(m_lock);
    m_initialized = false;
    m_object = object();
  }
};
