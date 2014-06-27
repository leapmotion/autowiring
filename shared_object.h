// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "stdafx.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include "guard_object.h"

template<class object> class guard_object;

///<summary>
///This class provides a standard thread-safe wrapper for data structures.
///Because instantiation may be separate from initialization, this class
///also tracks the initialization status.
///</summary>
///<remarks>
///This class supports two modes of use:
/// - Atomic assignments which can be used to create a consistent local copy,
///   or to modify the contents of shared_object.
/// - Held locks, via guard_object, which will block all other interactions.
///</remarks>
template<class object>
class shared_object {
  friend class guard_object<object>;

  //CONTRACT: If !m_initialized then m_object == object();
  mutable boost::mutex m_lock;
  bool m_initialized;
  object m_object;

public:
  ///<summary>
  ///Default constructor yielding initialized() == false.
  ///</summary>
  shared_object():
  m_initialized(false) {};

  ///<summary>
  ///Copy constructor yielding initialized() == _arg.initialized().
  ///</summary>
  shared_object(shared_object& _arg) {
    boost::lock_guard<boost::mutex> lock_arg(_arg.m_lock);
    m_initialized = _arg.m_initialized;
    m_object = _arg.m_object;
  }

  ///<summary>
  ///Assignment yielding initialized() == _rhs.initialized().
  ///</summary>
  shared_object& operator = (const shared_object& _rhs) {
    boost::lock_guard<boost::mutex> lock_this(m_lock);
    boost::lock_guard<boost::mutex> lock_rhs(_rhs.m_lock);
    m_initialized = _rhs.m_initialized;
    m_object = _rhs.m_object;
    return *this;
  }

  ///<summary>
  ///Initialization yielding initialized() == true.
  ///</summary>
  shared_object(const object& _arg):
  m_initialized(true),
  m_object(_arg) {}

  ///<summary>
  ///Assignment yielding initialized() == true.
  ///</summary>
  shared_object& operator = (const object& _rhs) {
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

  ///<summary>
  ///Creates a shared reference to this object that also maintains a lock
  //</summary>
  guard_object<object> hold() {
    return guard_object<object>(*this);
  }

  ///<summary>
  ///Tries to create a shared reference to this object that also maintains a lock,
  ///or returns an empty guard object.
  ///</summary>
  guard_object<object> try_hold() {
    if (m_lock.try_lock()) {
      return guard_object<object>(*this, boost::adopt_lock);
    }
    return guard_object<object>();
  }
};
