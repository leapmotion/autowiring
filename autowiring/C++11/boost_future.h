// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#if defined(BOOST_THREAD_FUTURE_HPP) && !defined(BOOST_THREAD_PROVIDES_FUTURE)
  #error Define BOOST_THREAD_PROVIDES_FUTURE before including boost/thread
#endif

#define BOOST_THREAD_PROVIDES_FUTURE
#include <boost/thread/future.hpp>

namespace std {
  using boost::future;
  using boost::future_status;
  using boost::promise;
  using boost::future_error;
  using boost::async;
  using boost::launch;
}
