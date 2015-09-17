// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <atomic>

namespace autowiring {

class spin_lock {
public:
  spin_lock(void) {
    m_lock.clear();
  }

private:
  mutable std::atomic_flag m_lock;

public:
  void lock(void) {
    // acquire lock
    while (m_lock.test_and_set(std::memory_order_acquire))
      // spin
      ;
  }

  bool try_lock(void) {
    // One shot attempt and return
    return !m_lock.test_and_set(std::memory_order_acquire);
  }

  void unlock(void) {
    // release lock
    m_lock.clear(std::memory_order_release);
  }

  // Can't be copied, moved, or assigned:
  spin_lock(spin_lock&&) = delete;
  spin_lock(const spin_lock&) = delete;
  void operator=(const spin_lock&) = delete;
  void operator=(spin_lock&&) = delete;
};

}
