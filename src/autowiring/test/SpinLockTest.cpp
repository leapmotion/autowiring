// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/spin_lock.h>
#include <gtest/gtest.h>
#include <mutex>
#include <thread>

TEST(SpinLockTest, RecursiveAcquireFails) {
  autowiring::spin_lock lock;
  ASSERT_TRUE(lock.try_lock()) << "Lock acquisition attempt did not succeed immediately where it should have been uncontended";
  ASSERT_FALSE(lock.try_lock()) << "Lock attempted succeeded after lock should have already been held";
  lock.unlock();
}

TEST(SpinLockTest, SimpleExclusion) {
  auto pLock = std::make_shared<autowiring::spin_lock>();
  pLock->lock();

  auto lockSucceeded = std::make_shared<bool>(false);
  std::thread([lockSucceeded, pLock] {
    *lockSucceeded = pLock->try_lock();
  }).join();
  ASSERT_FALSE(*lockSucceeded) << "A lock attempt made from another thread succeeded while the lock should have been held";
}

TEST(SpinLockTest, PathologicalExclusion) {
  auto pLock = std::make_shared<autowiring::spin_lock>();
  auto value = std::make_shared<std::atomic<int>>(0);
  auto success = std::make_shared<bool>(true);

  // Four threads, one per core, that loop 100x
  std::vector<std::thread> threads(4);
  for (auto& thread : threads)
    thread = std::thread(
      [=] {
        for (size_t i = 1000; i--;) {
          std::lock_guard<autowiring::spin_lock> lk(*pLock);
          ++(*value);
          if (*value != 1)
            *success = false;
          --(*value);
        }
      }
    );

  for (auto& thread : threads)
    if (thread.joinable())
      thread.join();

  // Verify the lock did everything it was supposed to do:
  ASSERT_TRUE(*success) << "Lock failed to exclude multi-access under pathological cases";
}

TEST(SpinLockTest, LockGuardBehavior) {
  autowiring::spin_lock lock;
  {
    std::lock_guard<autowiring::spin_lock> lk(lock);
    ASSERT_FALSE(lock.try_lock()) << "Mutual exclusion is not correctly assured during lock_guard lock";
  }
  ASSERT_TRUE(lock.try_lock()) << "Lock could not be reobtained after it should have been released";
}

TEST(SpinLockTest, UniqueLockBehavior) {
  autowiring::spin_lock lock;

  std::unique_lock<autowiring::spin_lock> outer;
  {
    std::unique_lock<autowiring::spin_lock> lk(lock);
    ASSERT_FALSE(lock.try_lock()) << "Mutual exclusion is not correctly assured during lock_guard lock";
    outer = std::move(lk);
  }
  ASSERT_FALSE(lock.try_lock()) << "Lock was incorrectly released during inner scope teardown";
}
