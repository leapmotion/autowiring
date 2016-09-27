// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once

#include <autoboost/atomic.hpp>
namespace std {
  using autoboost::atomic;
  using autoboost::atomic_flag;
  using autoboost::atomic_thread_fence;
  using autoboost::memory_order_relaxed;
  using autoboost::memory_order_consume;
  using autoboost::memory_order_acquire;
  using autoboost::memory_order_release;
  using autoboost::memory_order_acq_rel;
  using autoboost::memory_order_seq_cst;
#if !defined(ATOMIC_FLAG_INIT)
#define ATOMIC_FLAG_INIT
#endif
}
