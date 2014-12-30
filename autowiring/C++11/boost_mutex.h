// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/once.hpp>

namespace std {
  using AUTOWIRING_BOOST_NAME::mutex;
  using AUTOWIRING_BOOST_NAME::recursive_mutex;
  using AUTOWIRING_BOOST_NAME::lock_guard;
  using AUTOWIRING_BOOST_NAME::unique_lock;
  using AUTOWIRING_BOOST_NAME::condition_variable;
  using AUTOWIRING_BOOST_NAME::condition_variable_any;
  using AUTOWIRING_BOOST_NAME::cv_status;
  using AUTOWIRING_BOOST_NAME::once_flag;
  using AUTOWIRING_BOOST_NAME::call_once;
}
