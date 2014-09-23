// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include TYPE_TRAITS_HEADER

/*
 Variadic generalizations of boolean operations.
 Single argument cases are equal to the argument.
 Zero argument cases are defined by the requirement
 that the result is idempotent with respect to the
 boolean operation.
 */

/// <summary>Recursive OR</summary>
/// <remarks>
/// Construction requires explicit template declaration.
/// </remarks>
template<typename... Base>
struct rec_or {
  static_assert(sizeof...(Base) == 0, "Non-boolean type used in template");
  const bool value;
  constexpr rec_or():
   value(false)
  {};
  constexpr operator bool () const {
    return value;
  }
};
template<typename... Tail>
struct rec_or<bool, Tail...> {
  const bool value;
  constexpr rec_or(const bool& h, const Tail&... t):
    value(h || rec_or<Tail...>(t...).value)
  {}
  constexpr operator bool () const {
    return value;
  }
};

/// <summary>Variadic OR</summary>
/// <remarks>
/// Function enables deduction of implicit template.
/// </remarks>
template<typename... Args>
constexpr rec_or<Args...> var_or(Args... a) {
  return rec_or<Args...>(a...);
}


///<summary>Recursive AND</summary>
/// <remarks>
/// Construction requires explicit template declaration.
/// </remarks>
template<typename... Base>
struct rec_and {
  static_assert(sizeof...(Base) == 0, "Non-boolean type used in template");
  const bool value;
  constexpr rec_and():
   value(true)
  {};
  constexpr operator bool () const {
    return value;
  }
};
template<typename... Tail>
struct rec_and<bool, Tail...> {
  const bool value;
  constexpr rec_and(const bool& h, const Tail&... t):
    value(h && rec_and<Tail...>(t...).value)
  {}
  constexpr operator bool () const {
    return value;
  }
};

/// <summary>Variadic AND</summary>
/// <remarks>
/// Function enables deduction of implicit template.
/// </remarks>
template<typename... Args>
constexpr rec_and<Args...> var_and(Args... a) {
  return rec_and<Args...>(a...);
}
