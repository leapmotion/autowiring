// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ContextTrackingBm.h"
#include "Benchmark.h"
#include <autowiring/ContextMap.h>
#include <functional>
#include <thread>

static const size_t n = 100;

static std::vector<std::shared_ptr<CoreContext>> create(void) {
  // Create a bunch of subcontexts from here
  std::vector<std::shared_ptr<CoreContext>> contexts(n);
  for (size_t i = n; i--;)
    contexts[i] = AutoCreateContext();
  return contexts;
}

template<size_t N>
static void do_parallel_enum(Stopwatch& sw) {
  AutoCurrentContext ctxt;
  auto all = create();

  // Create threads which will cause contention:
  auto proceed = std::make_shared<bool>(true);
  for (size_t nParallel = N; nParallel--;) {
    std::thread([proceed, all, ctxt] {
      while (*proceed)
        for (auto cur : ContextEnumerator(ctxt))
          ;
    }).detach();
  }
  auto cleanup = MakeAtExit([&] { *proceed = false; });

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Perform parallel enumeration
  sw.Start();
  for (auto cur : ContextEnumerator(ctxt))
    ;
  sw.Stop(n);
}

Benchmark ContextTrackingBm::ContextEnum(void) {
  return {
    {
      "single",
      [](Stopwatch& sw) {
        auto all = create();

        // Now enumerate
        sw.Start();
        AutoCurrentContext ctxt;
        for (auto cur : ContextEnumerator(ctxt))
          ;
        sw.Stop(n);
      }
    },
    {
      "parallel",
      do_parallel_enum<1>
    },
    {
      "parallel x10",
      do_parallel_enum<1>
    }
  };
}

template<size_t N>
static void do_parallel_map(Stopwatch& sw) {
  // Create a bunch of subcontexts from here and put them in a map
  auto all = create();
  ::ContextMap<size_t> mp;
  for (size_t i = N; i < all.size(); i++)
    mp.Add(i, all[i]);

  // Now the threads that will make progress hard:
  auto proceed = std::make_shared<bool>(true);
  for (size_t i = N; i--;)
    std::thread([proceed, mp] {
      while (*proceed)
        for (const auto& cur : mp)
          ;
    }).detach();
  auto cleanup = MakeAtExit([&] { *proceed = false; });

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Enumerate the map while iteration is underway
  sw.Start();
  for (auto& cur : mp)
    ;
  sw.Stop(n);
}

Benchmark ContextTrackingBm::ContextMap(void) {
  static const size_t n = 100;

  return {
    {
      "single",
      [](Stopwatch& sw) {
        // Create a bunch of subcontexts from here and put them in a map
        auto all = create();
        ::ContextMap<size_t> mp;
        for (size_t i = 0; i < all.size(); i++)
          mp.Add(i, all[i]);

        // Just enumerate the map, not much to it
        sw.Start();
        for (auto& cur : mp)
          ;
        sw.Stop(n);
      }
    },
    {
      "parallel",
      do_parallel_map<1>
    },
    {
      "parallel x10",
      do_parallel_map<10>
    }
  };

}
