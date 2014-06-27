// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "stdafx.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include "atomic_object.h"

///<summary>
///This class implements the atomic_object interface to a shared reference.
///This will enable the construction of unlock_objects that combine the existential
///guarantee of a shared_ptr with the immutability guarantee of a lock_guard.
///</summary>
///<remarks>
///This class guarantees the existence of a referenced atomic_object.
///While the referenced atomic_object is thread-safe, this class itself is not,
///and is expected to be used as a local reference to a shared resource.
///</remarks>
template<class object>
class shared_object {
  friend class unlock_object<object>;

  //CONTRACT: m_share == true
  std::shared_ptr<atomic_object<object>> m_share;

public:
  ///<summary>
  ///Default constructor instiates an uninitialized atomic_object.
  ///</summary>
  shared_object():
  m_share(new atomic_object<object>()) {}

  ///<summary>
  ///Copy constructor instantiates a atomic_object initialized by _arg.
  ///</summary>
  ///<remarks>
  ///This shares the referenced atomic_object.
  ///To make a deep copy use shared_object(const atomic_object& _arg).
  ///</remakrs>
  shared_object(const shared_object<object>& _arg):
  m_share(_arg.m_share) {}

  ///<summary>
  ///Copy constructor instiates a atomic_object initialized by _arg.
  ///</summary>
  shared_object(const object& _arg):
  m_share(new atomic_object<object>(_arg)) {}

  ///<summary>
  ///Assignment to current atomic_object.
  ///</summary>
  ///<remarks>
  ///This shares the referenced atomic_object.
  ///To make a deep copy use shared_object(const atomic_object& _arg).
  ///</remakrs>
  shared_object<object>& operator = (const shared_object<object>& _arg) {
    m_share = _arg.m_share;
    return *this;
  }

  ///<summary>
  ///Assignment to current atomic_object.
  ///</summary>
  shared_object<object>& operator = (const object& _arg) {
    *m_share = _arg;
    return *this;
  }

  ///<summary>
  ///Cast by copying current state
  ///</summary>
  operator object() const {
    return static_cast<object>(*m_share);
  }

  ///<return>True if the object was not assigned default values</return>
  bool initialized() const {
    return m_share->initialized();
  }

  ///<summary>
  ///Atomic copy of this location to argument location, only if this has location.
  ///</summary>
  ///<return>True if the object was not assigned default values</return>
  bool initialized(object& _use) {
    return m_share->initialized(_use);
  }

  ///<summary>
  ///Reset using default constructor yielding initialized() == false.
  ///</summary>
  void reset() {
    m_share->reset();
  }
};
