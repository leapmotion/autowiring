// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once

namespace autowiring {
  template<int... Ns>
  struct sum;

  template<int N, int... Ns>
  struct sum<N, Ns...> :
    sum<Ns...>
  {
    static const size_t value = N + sum<Ns...>::value;
  };

  template<>
  struct sum<>
  {
    static const size_t value = 0;
  };
}
