// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once

#define AUTOBOOST_THREAD_PROVIDES_FUTURE
#include <autoboost/thread/future.hpp>

namespace std {
  using autoboost::future;
  using autoboost::future_status;
  using autoboost::promise;
  using autoboost::future_error;
  using autoboost::async;
  using autoboost::launch;
}
