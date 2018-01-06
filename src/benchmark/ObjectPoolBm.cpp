// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ObjectPoolBm.h"
#include "Benchmark.h"
#include <autowiring/ObjectPool.h>
#include <functional>
#include <vector>

static const size_t n = 100;

// Dummy struct of ten integers--needed because unique_ptr<int[10]> doesn't compile
struct tenPack { int dummy[10]; };

// Type that will perform a secondary allocation on construction
struct filling_vector {
  filling_vector(void) {
    other.resize(100);
  }

  std::vector<int> other;
};

template<typename T>
void profile_basic(Stopwatch& sw) {
  std::vector<std::unique_ptr<T>> mem;

  // Initial set of allocations:
  for (size_t i = n; i--;)
    mem.emplace_back(new T);
  mem.clear();

  // Now simulate something where an object pool might have been handy
  for (size_t k = n; k--;) {
    sw.Start();
    for (size_t i = n; i--;)
      mem.emplace_back(new T);
    sw.Stop(n * n);
    mem.clear();
  }
}

template<typename T>
void profile_pool(Stopwatch& sw) {
  ObjectPool<T> pool;
  pool.Preallocate(n);

  // Now simulate something where an object pool might have been handy
  std::vector<std::shared_ptr<T>> mem;
  for (size_t k = n; k--;) {
    sw.Start();
    for (size_t i = n; i--;)
      mem.emplace_back(pool());
    sw.Stop(n * n);
    mem.clear();
  }
}

Benchmark ObjectPoolBm::Allocation(void) {
  return {
    { "basic", &profile_basic<tenPack> },
    { "complex", &profile_basic<filling_vector> },
    { "pool_basic", &profile_pool<tenPack> },
    { "pool_complex", &profile_pool<filling_vector> }
  };
}
