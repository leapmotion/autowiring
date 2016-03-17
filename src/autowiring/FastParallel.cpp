// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "FastParallel.h"

using namespace autowiring;

FastParallel::FastParallel(size_t concurrency) :
  numThreads(concurrency)
{}

FastParallel::~FastParallel(void) {
  stop = true;
  std::lock_guard<std::mutex>{mtx};
  condition.notify_all();
  for (auto& thread : threads) {
    thread.join();
  }
}

void FastParallel::Resize(size_t concurrency) {
  std::lock_guard<std::mutex> lock(mtx);
  if (concurrency > numThreads) {
    numThreads = concurrency;
  }
}

void FastParallel::WorkerLoop(void) {
  int index;
  while (!stop) {
    {
      std::unique_lock<std::mutex> lock(mtx);
      condition.wait(lock, [this] { return next > 0 || stop; });
    }
    while ((index = --next) >= 0) {
      workerFunc(index);
      if (--remaining == 0) {
        std::lock_guard<std::mutex>{mtx};
        condition.notify_all();
      }
    }
  }
}

void FastParallel::Parallelize(int num, std::function<void(int)>&& f) {
  std::unique_lock<std::mutex> lock(mtx);
  workerFunc = std::move(f);
  remaining = num;
  next = num;

  while (threads.size() < numThreads) {
    threads.emplace_back(&FastParallel::WorkerLoop, this);
  }
  lock.unlock();
  condition.notify_all();

  int index;
  while ((index = --next) >= 0) {
    workerFunc(index);
    if (--remaining == 0) {
      return; // Nothing to wait on if we are the last thread to process work
    }
  }
  lock.lock();
  condition.wait(lock, [this] { return remaining == 0; });
}
