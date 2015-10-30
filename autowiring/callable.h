// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <memory>

namespace autowiring {
  // Callable wrapper type, always invoked in a synchronized context
  struct callable_base {
    virtual ~callable_base(void) {}
    virtual void operator()() {}
    callable_base* m_pFlink = nullptr;
  };

  template<typename Fn>
  struct callable :
    callable_base
  {
    callable(Fn&& fn) : fn(std::move(fn)) {}
    Fn fn;
    void operator()() override { fn(); }
  };
}
