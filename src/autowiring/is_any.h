// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include "C++11/cpp11.h"
#include TYPE_TRAITS_HEADER

namespace autowiring {

/// <summary>
/// Check if any T is true
/// </summary>
/// <remarks>
/// Check is_any<...>::value for result
/// </remarks>
template<bool... val>
struct is_any {
  static const bool value = false;
};

template<bool Head, bool... Tail>
struct is_any<Head, Tail...> {
  static const bool value = Head || is_any<Tail...>::value;
};

/// <summary>
/// Extended version of is_same for any number of types
/// </summary>
/// <remarks>
/// Check is_any_same<...>::value for result
/// </remarks>
template<typename T, typename... Us>
struct is_any_same;

template<typename T>
struct is_any_same<T> {
  static const bool value = false;
};

template<typename T, typename U, typename... Us>
struct is_any_same<T, U, Us...> {
  static const bool value = std::is_same<T, U>::value || is_any_same<T, Us...>::value;
};

template<typename T, typename... Us>
struct is_any_repeated;

template<typename T>
struct is_any_repeated<T>:
  std::false_type
{};

template<typename T, typename U, typename... Us>
struct is_any_repeated<T, U, Us...>:
  std::integral_constant<bool,
    is_any_same<T, U, Us...>::value ||
    is_any_repeated<U, Us...>::value
  >
{};

}
