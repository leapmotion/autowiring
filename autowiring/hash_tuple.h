// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once
#include STL_TUPLE_HEADER

namespace std
{
  template<class... Types>
  struct hash<std::tuple<Types...>> {
    static uint64_t hash_combine(const size_t& lhs, const size_t& rhs) {
      //return lhs ^ rhs;
      //static const size_t salt = 0x9e3779b9;
      static const uint64_t salt = 0x278dde6e5fd29e00;
      return rhs ^ (salt + (lhs << 6) + (lhs >> 2));
      // Adding pseudo-random bits reduces collisions between consecutive values
      // 0 < 1/phi < 1 so the denominator includes a shift.
      // phi = (1 + sqrt(5)) / 2
      // 2^32 / phi = 0x9e3779b9
      // 2^64 / phi = 0x278dde6e5fd29e00
    }

    // Base: recursion from empty tail
    template<int N>
    static size_t hash_tuple(const std::tuple<Types...>& value) { return 0; }

    // General: combine hash<head> with recursion on tail
    template<int N, class Head, class... Tail>
    static size_t hash_tuple(const std::tuple<Types...>& value) {
      return (size_t)hash_combine(
        std::hash<Head>()(
          std::get<N - sizeof...(Tail) - 1>(value)),
          hash_tuple<N, Tail...>(value)
        );
    }

  public:
    size_t operator()(const std::tuple<Types...>& value) const {
      return hash_tuple<sizeof...(Types), Types...>(value);
    }
  };
}
