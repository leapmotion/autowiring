// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once

namespace autowiring {
  template<class... Args>
  void noop(Args...) {}

  template<class... Args>
  struct noop_t {
    void operator()(Args...) const {}
  };
}
