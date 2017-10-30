// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ContextSearchBm.h"
#include "Benchmark.h"
#include <algorithm>
#include <unordered_map>

Benchmark ContextSearchBm::Search(void) {
  static const size_t n = 10000;

  // Prime object insertion, a few tests require this
  AutoRequired<Foo>();

  return{
    {
      "std::unordered_map::operator[]",
      [](Stopwatch& sw) {
        std::unordered_map<int, int> ref;
        ref[0] = 212;
        ref[1] = 21;

        sw.Start();
        for (size_t i = n; i--;)
          ref[i % 2];
        sw.Stop(n);
      }
    },
    {
      "Autowired<T>",
      [](Stopwatch& sw) {
        sw.Start();
        for (size_t i = n; i--;)
          Autowired<Foo>();
        sw.Stop(n);
      }
    },
    {
      "AutowiredFast<T>",
      [](Stopwatch& sw) {
        sw.Start();
        for (size_t i = n; i--;)
          AutowiredFast<Foo>();
        sw.Stop(n);
      }
    }
  };
}

template<int N>
struct dummy:
  public ContextMember
{};

void InjectDummy(void) {
  Autowired<dummy<1>>();
  Autowired<dummy<2>>();
  Autowired<dummy<3>>();
  Autowired<dummy<4>>();
  Autowired<dummy<5>>();
  Autowired<dummy<6>>();
  Autowired<dummy<7>>();
  Autowired<dummy<8>>();
  Autowired<dummy<9>>();
  Autowired<dummy<10>>();
  Autowired<dummy<11>>();
  Autowired<dummy<12>>();
  Autowired<dummy<13>>();
  Autowired<dummy<14>>();
  Autowired<dummy<15>>();
  Autowired<dummy<16>>();
  Autowired<dummy<17>>();
  Autowired<dummy<18>>();
  Autowired<dummy<19>>();
  Autowired<dummy<20>>();
  Autowired<dummy<21>>();
  Autowired<dummy<22>>();
  Autowired<dummy<23>>();
  Autowired<dummy<24>>();
  Autowired<dummy<25>>();
};

Benchmark ContextSearchBm::Cache(void)
{
  std::chrono::nanoseconds baseline(0);
  std::chrono::nanoseconds benchmark(0);
  static const size_t n = 100;

  return Benchmark{
    {
      "Inject+Miss",
      [] (Stopwatch& sw) {
        std::vector<std::shared_ptr<CoreContext>> ctxts(n);
        for (size_t i = 0; i < n; i++)
          ctxts[i] = AutoCreateContext();

        sw.Start();
        for(auto& ctxt : ctxts) {
          CurrentContextPusher pshr(ctxt);
          InjectDummy();
        }
        sw.Stop(25 * n);
      }
    },
    {
      "Inject",
      [] (Stopwatch& sw) {
        std::vector<std::shared_ptr<CoreContext>> ctxts(n);
        for (size_t i = 0; i < n; i++) {
          ctxts[i] = AutoCreateContext();
          CurrentContextPusher pshr(ctxts[i]);
          InjectDummy();
        }

        sw.Start();
        for (size_t i = n; i--;) {
          CurrentContextPusher pshr(ctxts[i]);
          InjectDummy();
        }
        sw.Stop(25 * n);
      }
    }
  };
}

Benchmark ContextSearchBm::Fast(void) {
  // All of these tests will operate in the same context:
  AutoCreateContext ctxt;
  CurrentContextPusher pshr(ctxt);
  static const size_t n = 500;

  // Prime all memos:
  InjectDummy();

  return Benchmark{
    {
      "unordered_map<typeinfo, shared_ptr>",
      [](Stopwatch& sw) {
        static const std::type_info* tis [] = {
          &typeid(dummy<1>),
          &typeid(dummy<2>),
          &typeid(dummy<3>),
          &typeid(dummy<4>),
          &typeid(dummy<5>),
          &typeid(dummy<6>),
          &typeid(dummy<7>),
          &typeid(dummy<8>),
          &typeid(dummy<9>),
          &typeid(dummy<10>),
          &typeid(dummy<11>),
          &typeid(dummy<12>),
          &typeid(dummy<13>),
          &typeid(dummy<14>),
          &typeid(dummy<15>),
          &typeid(dummy<16>),
          &typeid(dummy<17>),
          &typeid(dummy<18>),
          &typeid(dummy<19>),
          &typeid(dummy<20>),
          &typeid(dummy<21>),
          &typeid(dummy<22>),
          &typeid(dummy<23>),
          &typeid(dummy<24>),
          &typeid(dummy<25>)
        };

        std::unordered_map<std::type_index, std::shared_ptr<bool>> v;
        for (const auto* ti : tis)
          v[*ti] = std::make_shared<bool>(true);

        sw.Start();
        for (int i = 0; i < n; ++i) {
          v.find(*tis[i % 25]);
        }
        sw.Stop(n);
      }
    },
    {
      "Autowired<T>",
      [](Stopwatch& sw) {
        sw.Start();
        for (int i = 0; i < n; ++i)
          InjectDummy();
        sw.Stop(n * 25);
      }
    },
    {
      "AutowiredFast<T>",
      [](Stopwatch& sw) {
        sw.Start();
        for (int i = 0; i < n; ++i) {
          AutowiredFast<dummy<1>>();
          AutowiredFast<dummy<2>>();
          AutowiredFast<dummy<3>>();
          AutowiredFast<dummy<4>>();
          AutowiredFast<dummy<5>>();
          AutowiredFast<dummy<6>>();
          AutowiredFast<dummy<7>>();
          AutowiredFast<dummy<8>>();
          AutowiredFast<dummy<9>>();
          AutowiredFast<dummy<10>>();
          AutowiredFast<dummy<11>>();
          AutowiredFast<dummy<12>>();
          AutowiredFast<dummy<13>>();
          AutowiredFast<dummy<14>>();
          AutowiredFast<dummy<15>>();
          AutowiredFast<dummy<16>>();
          AutowiredFast<dummy<17>>();
          AutowiredFast<dummy<18>>();
          AutowiredFast<dummy<19>>();
          AutowiredFast<dummy<20>>();
          AutowiredFast<dummy<21>>();
          AutowiredFast<dummy<22>>();
          AutowiredFast<dummy<23>>();
          AutowiredFast<dummy<24>>();
          AutowiredFast<dummy<25>>();
        }
        sw.Stop(n * 25);
      }
    }
  };
}
