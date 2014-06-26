// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "stdafx.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include "shared_object.h"

template<class Object> class shared_object;

///<summary>
///A reference to a shared_object that simultaneously holds the lock
///</summary>
///<remarks>
///This object may be copied multiple times and will maintain a lock
///on the shared_object so long as any copy is in existence.
///</remarks>
template<class Object>
class guard_object:
private std::shared_ptr<Object> {
  friend class shared_object<Object>;

  std::shared_ptr<boost::lock_guard<boost::mutex>> m_hold;

  guard_object(shared_object<Object>& _arg, boost::adopt_lock_t _tag):
  m_hold(new boost::lock_guard<boost::mutex>(_arg.m_lock, _tag)) {
    reset(&_arg.m_object);
  }

public:
  guard_object() {}

  ~guard_object() {
    reset();
  }

  explicit guard_object(shared_object<Object>& _arg):
  m_hold(new boost::lock_guard<boost::mutex>(_arg.m_lock)) {
    std::shared_ptr<Object>::reset(&_arg.m_object);
  }

  guard_object(const guard_object<Object>& _arg) {
    m_hold = _arg.m_hold;
    std::shared_ptr<Object>::operator = (_arg);
  }

  guard_object& operator = (const guard_object& _rhs) {
    m_hold = _rhs.m_hold;
    std::shared_ptr<Object>::operator = (_rhs);
  }

  void reset() {
    m_hold.reset();
    std::shared_ptr<Object>::reset();
  }

  void reset(shared_object<Object>& _arg) {
    m_hold.reset(new boost::lock_guard<boost::mutex>(_arg.m_lock));
    std::shared_ptr<Object>::reset(&_arg.m_object);
  }

  ///Expose only methods that can not modify the Object reference
  using std::shared_ptr<Object>::operator*;
  using std::shared_ptr<Object>::operator->;
  using std::shared_ptr<Object>::operator bool;
  using std::shared_ptr<Object>::use_count;
};
