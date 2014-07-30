// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <boost/atomic.hpp>
namespace std {
  using boost::atomic;
  using boost::atomic_flag;
  using boost::memory_order_relaxed;
  using boost::memory_order_consume;
  using boost::memory_order_acquire;
  using boost::memory_order_release;
  using boost::memory_order_acq_rel;
  using boost::memory_order_seq_cst;
#if !defined(ATOMIC_FLAG_INIT)
#define ATOMIC_FLAG_INIT
#endif
}
