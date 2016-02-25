// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "unlock_object.h"
#include MEMORY_HEADER
#include MUTEX_HEADER
#include TYPE_TRAITS_HEADER

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
  mutable lock m_lock;
  object m_object;

public:
  ///<summary>
  ///Default constructor yielding initialized() == false.
  ///</summary>
  atomic_object() {};

  ///<summary>
  ///Thread-safe copy constructor
  ///</summary>
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

    // Use memory well-ordering to establish a lock heirarchy
    if(this < &source) {
      m_lock.lock();
      source.m_lock.lock();
    }
    else {
      source.m_lock.lock();
      m_lock.lock();
    }

    m_object = source.m_object;

    m_lock.unlock();
    source.m_lock.unlock();
    return *this;
  }

  ///<summary>
  ///Assignment overload
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
