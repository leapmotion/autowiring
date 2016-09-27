// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once

#include <autoboost/thread/mutex.hpp>
#include <autoboost/thread/recursive_mutex.hpp>
#include <autoboost/thread/lock_guard.hpp>
#include <autoboost/thread/condition_variable.hpp>
#include <autoboost/thread/condition_variable.hpp>
#include <autoboost/thread/once.hpp>

namespace std {
  using autoboost::mutex;
  using autoboost::recursive_mutex;
  using autoboost::lock_guard;
  using autoboost::unique_lock;
  using autoboost::condition_variable;
  using autoboost::condition_variable_any;
  using autoboost::cv_status;
  using autoboost::once_flag;
  using autoboost::call_once;
}
