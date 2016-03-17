// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace autowiring {

class FastParallel {
public:
  FastParallel(size_t concurrency = 1);
  ~FastParallel(void);

  void Resize(size_t concurrency);
  void Parallelize(int num, std::function<void(int)>&& f);

private:
  mutable std::mutex mtx;
  std::condition_variable condition;
  std::vector<std::thread> threads;
  std::function<void(int)> workerFunc;
  std::atomic<bool> stop{false};
  std::atomic<int> next{0};
  std::atomic<int> remaining{0};
  size_t numThreads;

  void WorkerLoop(void);
};

}//namespace autowiring
