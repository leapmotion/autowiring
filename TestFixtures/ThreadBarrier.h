// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include MUTEX_HEADER
#include CHRONO_HEADER

// An implemenetation of boost::barrier
// Blocks until N threads call "wait" on this object, then wakes up
// waiting threads and resets itself
class ThreadBarrier {
public:
  explicit ThreadBarrier(int n):
    m_barrierLimit(n),
    m_numWaiters(0),
    m_lastReset(std::chrono::high_resolution_clock::now())
  {}

  virtual ~ThreadBarrier(){}

  ThreadBarrier(const ThreadBarrier&) = delete;
  ThreadBarrier& operator=(const ThreadBarrier&) = delete;

  // Waits until "m_barrierLimit" threads call "wait"
  void wait(){
    std::unique_lock<std::mutex> lk(m_mutex);
    ++m_numWaiters;
    
    auto now = std::chrono::high_resolution_clock::now();

    if (m_numWaiters == m_barrierLimit) {
      m_numWaiters = 0;
      m_lastReset = now;
      m_cv.notify_all();
    } else {
      m_cv.wait(lk, [this, now] {
        return now < m_lastReset;
      });
    }
  }

private:
  // The number of times "wait" must be called before all threads are unblocked
  const int m_barrierLimit;

  // The number of current waiters
  int m_numWaiters;
  
  // Time of last reset
  std::chrono::high_resolution_clock::time_point m_lastReset;

  // Synchronization for the barrier
  std::mutex m_mutex;
  std::condition_variable m_cv;
};
