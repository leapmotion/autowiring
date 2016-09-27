// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include STL_TUPLE_HEADER
#include "index_tuple.h"

namespace std
{
  template<typename... Types>
  struct hash<std::tuple<Types...>> {

    template<typename T, typename... Ts>
    static uint64_t hash_combine(const T& head, const Ts&... tail) {
      // Get hash for head and tail
      uint64_t head_hash = std::hash<T>()(head);
      uint64_t tail_hash = hash_combine(tail...);

      // Add a pinch of salt and stir
      static const uint64_t salt = 0x278dde6e5fd29e00;
      return head_hash ^ (salt + (tail_hash << 6) + (tail_hash >> 2));
    }

    // base case to hash std::tuple<>
    static uint64_t hash_combine(void) {
      return 0;
    }

    // Hash each element in the tuple and mix them together
    template<int... I>
    static size_t hash_tuple(index_tuple<I...>, const std::tuple<Types...>& value) {
      return static_cast<size_t>(hash_combine(std::get<I>(value)...));
    }

  public:
    size_t operator()(const std::tuple<Types...>& value) const {
      return hash_tuple(typename make_index_tuple<sizeof...(Types)>::type(), value);
    }
  };
}
