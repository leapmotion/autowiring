// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#if defined(BOOST_THREAD_FUTURE_HPP) && !defined(BOOST_THREAD_PROVIDES_FUTURE)
  #error Define BOOST_THREAD_PROVIDES_FUTURE before including boost/thread
#endif

#define BOOST_THREAD_PROVIDES_FUTURE
#include <boost/thread/future.hpp>

namespace std {
  using AUTOWIRING_BOOST_NAME::future;
  using AUTOWIRING_BOOST_NAME::future_status;
  using AUTOWIRING_BOOST_NAME::promise;
  using AUTOWIRING_BOOST_NAME::future_error;
  using AUTOWIRING_BOOST_NAME::async;
  using AUTOWIRING_BOOST_NAME::launch;
}
