// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include <mutex>
#include <condition_variable>

// An implemenetation of boost::barrier
// Blocks until N threads call "wait" on this object, then wakes up
// waiting threads and resets itself
class ThreadBarrier {
public:
  explicit ThreadBarrier(int n):
    m_barrierLimit(n),
    m_numWaiters(0)
  {}

  virtual ~ThreadBarrier(){}

  ThreadBarrier(const ThreadBarrier&) = delete;
  ThreadBarrier& operator=(const ThreadBarrier&) = delete;

  // Waits until "m_barrierLimit" threads call "wait"
  void wait(){
    std::unique_lock<std::mutex> lk(m_mutex);
    ++m_numWaiters;

    if (m_numWaiters == m_barrierLimit) {
      m_cv.notify_all();
      m_numWaiters = 0;
    } else {
      m_cv.wait(lk);
    }
  }

private:
  // The number of times "wait" must be called before all threads are unblocked
  const int m_barrierLimit;

  // The number of current waiters
  int m_numWaiters;

  // Synchronization for the barrier
  std::mutex m_mutex;
  std::condition_variable m_cv;
};
