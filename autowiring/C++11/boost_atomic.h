// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <boost/atomic.hpp>
namespace std {
  using AUTOWIRING_BOOST_NAME::atomic;
  using AUTOWIRING_BOOST_NAME::atomic_flag;
  using AUTOWIRING_BOOST_NAME::memory_order_relaxed;
  using AUTOWIRING_BOOST_NAME::memory_order_consume;
  using AUTOWIRING_BOOST_NAME::memory_order_acquire;
  using AUTOWIRING_BOOST_NAME::memory_order_release;
  using AUTOWIRING_BOOST_NAME::memory_order_acq_rel;
  using AUTOWIRING_BOOST_NAME::memory_order_seq_cst;
#if !defined(ATOMIC_FLAG_INIT)
#define ATOMIC_FLAG_INIT
#endif
}
