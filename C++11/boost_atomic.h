// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _BOOST_ATOMIC_H
#define _BOOST_ATOMIC_H

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

#endif
