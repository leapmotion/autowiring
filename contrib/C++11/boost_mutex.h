// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/once.hpp>

namespace std {
  using boost::mutex;
  using boost::recursive_mutex;
  using boost::lock_guard;
  using boost::unique_lock;
  using boost::condition_variable;
  using boost::condition_variable_any;
  using boost::cv_status;
  using boost::once_flag;
  using boost::call_once;
}
