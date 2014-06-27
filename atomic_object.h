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
///This class allows atomic assignments which can be used to create a
///mutable object copies, or to modify the contents of atomic_object.
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
  ///Copy constructor yielding initialized() == _arg.initialized().
  ///</summary>
  atomic_object(atomic_object& _arg) {
    boost::lock_guard<boost::mutex> lock_arg(_arg.m_lock);
    m_initialized = _arg.m_initialized;
    m_object = _arg.m_object;
  }

  ///<summary>
  ///Initialization yielding initialized() == true.
  ///</summary>
  atomic_object(const object& _arg):
  m_initialized(true),
  m_object(_arg) {}

  ///<summary>
  ///Assignment yielding initialized() == _rhs.initialized().
  ///</summary>
  atomic_object& operator = (const atomic_object& _rhs) {
    boost::lock_guard<boost::mutex> lock_this(m_lock);
    boost::lock_guard<boost::mutex> lock_rhs(_rhs.m_lock);
    m_initialized = _rhs.m_initialized;
    m_object = _rhs.m_object;
    return *this;
  }

  ///<summary>
  ///Assignment yielding initialized() == true.
  ///</summary>
  atomic_object& operator = (const object& _rhs) {
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
