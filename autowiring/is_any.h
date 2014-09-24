// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include TYPE_TRAITS_HEADER

/// <summary>
/// Check if any T::value is true
/// </summary>
/// <remarks>
/// Check is_any<...>::value for result
/// </remarks>
template<typename... T>
struct is_any{
  static const bool value = false;
};

template<typename Head, typename... Tail>
struct is_any<Head, Tail...>{
  static const bool value = Head::value || is_any<Tail...>::value;
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