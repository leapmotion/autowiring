// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "stdafx.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include "shared_object.h"

template<class object> class shared_object;

///<summary>
///A reference to a shared_object that simultaneously holds the lock.
///</summary>
///<remarks>
///The construction of a guard object establishes initialization, since
///the expectation is that the referenced object will be modified.
///</remarks>
template<class object>
class guard_object:
private std::shared_ptr<object> {
  friend class shared_object<object>;

  std::shared_ptr<boost::lock_guard<boost::mutex>> m_hold;

  guard_object(shared_object<object>& _arg, boost::adopt_lock_t _tag):
  m_hold(new boost::lock_guard<boost::mutex>(_arg.m_lock, _tag)) {
    std::shared_ptr<object>::reset(&_arg.m_object, [](object*){});
    _arg.m_initialized = true;
  }

public:
  ///<summary>
  ///Default constructor, yielding guard_object<object>::operator bool == false;
  ///</summary>
  guard_object() {}

  ///<summary>
  ///Construction from shared_object references the object and maintains a lock.
  ///</summary>
  explicit guard_object(shared_object<object>& _arg):
  m_hold(new boost::lock_guard<boost::mutex>(_arg.m_lock)) {
    std::shared_ptr<object>::reset(&_arg.m_object, [](object*){});
    _arg.m_initialized = true;
  }

  ///<summary>
  ///Copy constructor shares the reference and persists the existing guard lock.
  ///</summary>
  guard_object(const guard_object<object>& _arg) {
    m_hold = _arg.m_hold;
    std::shared_ptr<object>::operator = (_arg);
  }

  ///<summary>
  ///Assignment shares the reference and persists the existing guard lock.
  ///</summary>
  guard_object& operator = (const guard_object& _rhs) {
    m_hold = _rhs.m_hold;
    std::shared_ptr<object>::operator = (_rhs);
    return *this;
  }

  ///<summary>
  ///Reset releases reference and lock, yielding guard_object<object>::operator bool == false;
  ///</summary>
  void reset() {
    m_hold.reset();
    std::shared_ptr<object>::reset();
  }

  ///Expose only methods that can not modify the object reference
  using std::shared_ptr<object>::operator*;
  using std::shared_ptr<object>::operator->;
  using std::shared_ptr<object>::operator bool;
  using std::shared_ptr<object>::use_count;
};
