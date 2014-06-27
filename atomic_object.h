// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "stdafx.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>

template<class object> class shared_object;
template<class object> class unlock_object;

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
template<class object>
class atomic_object {
  friend class shared_object<object>;
  friend class unlock_object<object>;

  //CONTRACT: If !m_initialized then m_object == object();
  mutable boost::mutex m_lock;
  bool m_initialized;
  object m_object;

public:
  ///<summary>
  ///Default constructor yielding initialized() == false.
  ///</summary>
  atomic_object():
  m_initialized(false) {};

  ///<summary>
  ///Initialization yielding initialized() == true.
  ///</summary>
  ///<remarks>
  ///Intermediate copies can be avoided during construction using:
  /// atomic_object<object> target(*unlock_object<object>(source));
  ///</remarks>
  atomic_object(const object& _arg):
  m_initialized(true),
  m_object(_arg) {}

  ///<summary>
  ///Assignment yielding initialized() == true.
  ///</summary>
  ///<remarks>
  ///Intermediate copies can be avoided during assignment using:
  /// target = *unlock_object<object>(source);
  ///</remarks>
  atomic_object<object>& operator = (const object& _rhs) {
    boost::lock_guard<boost::mutex> lock_this(m_lock);
    m_initialized = true;
    m_object = _rhs;
    return *this;
  }

  ///<summary>
  ///Cast by copying current state
  ///</summary>
  operator object() const {
    boost::lock_guard<boost::mutex> lock_this(m_lock);
    return m_object;
  }

  ///<return>True if the object was not assigned default values</return>
  bool initialized() const {
    boost::lock_guard<boost::mutex> lock_this(m_lock);
    return m_initialized;
  }

  ///<summary>
  ///Atomic copy of this location to argument location, only if this has location.
  ///</summary>
  ///<return>True if the object was not assigned default values</return>
  bool initialized(object& _use) {
    boost::lock_guard<boost::mutex> lock_this(m_lock);
    if (m_initialized)
      _use = m_object;
    return m_initialized;
  }

  ///<summary>
  ///Reset using default constructor yielding initialized() == false.
  ///</summary>
  void reset() {
    boost::lock_guard<boost::mutex> lock_this(m_lock);
    m_initialized = false;
    m_object = object();
  }
};
