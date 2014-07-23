#ifndef _BOOST_FUTURE_HEADER_H
#define _BOOST_FUTURE_HEADER_H

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

#endif