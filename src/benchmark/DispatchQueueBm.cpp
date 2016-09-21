// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "DispatchQueueBm.h"
#include "Benchmark.h"
#include <autowiring/CoreThread.h>
#include FUTURE_HEADER
#include <thread>

Benchmark DispatchQueueBm::Dispatch(void) {
  static const size_t n = 10000;

  return Benchmark{
    {
      "std::list<std::function>",
      [] (Stopwatch& sw) {
        std::mutex lock;
        std::condition_variable cv;
        std::list<std::function<void()>> fns;
        bool proceed = true;

        // Launch a dispatcher that will simulate a naive dispatch queue based on std::list
        std::future<void> dispatcher = std::async(
          std::launch::deferred,
          [&] {
            while (proceed) {
              std::function<void()> fn;

              {
                std::unique_lock<std::mutex> lk(lock);
                cv.wait(lk, [&] { return !proceed || !fns.empty(); });
                if (!proceed)
                  return;

                fn = fns.front();
                fns.pop_front();
              }
              fn();
            }
          }
        );

        size_t x = 0;
        std::string s = "Hello world";
        auto l = [&x, s] { x += s.length(); };

        sw.Start();
        for (size_t i = n; i--;) {
          std::lock_guard<std::mutex>{lock},
          fns.push_back(l);
          cv.notify_all();
        }
        proceed = false;
        cv.notify_all();
        dispatcher.wait();
        sw.Stop(n);
      }
    },
    {
      "DispatchQueue::operator+=",
      [](Stopwatch& sw) {
        AutoCreateContext ctxt;
        CurrentContextPusher pshr(ctxt);
        AutoRequired<CoreThread> ct;
        ctxt->Initiate();

        size_t x = 0;
        std::string s = "Hello world";
        auto l = [&x, s] { x += s.length(); };

        sw.Start();
        for (size_t i = n; i--;)
          *ct += l;
        ctxt->SignalShutdown();
        ctxt->Wait();
        sw.Stop(n);
      }
    }
  };
}
