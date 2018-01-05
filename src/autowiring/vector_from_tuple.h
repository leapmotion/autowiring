// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include "C++11/cpp11.h"
#include TUPLE_HEADER
#include <vector>

namespace autowiring {
  template<typename T, typename... Ts, int... Ns>
  std::vector<T*> ptr_vector_from_tuple(const std::tuple<Ts...>& value, std::index_sequence<Ns...>) {
    return {
      static_cast<T*>(&std::get<Ns>(value))...
    };
  }

  template<typename T, typename... Ts>
  std::vector<T*> ptr_vector_from_tuple(const std::tuple<Ts...>& value) {
    return ptr_vector_from_tuple<T>(
      value,
      typename std::make_index_sequence<sizeof...(Ts)>{}
    );
  }
}
