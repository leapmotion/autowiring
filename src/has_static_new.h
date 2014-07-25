// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include TYPE_TRAITS_HEADER

/// <summary>
/// Utility helper structure for types which have a factory New routine
/// </summary>
/// <remarks>
/// A factory New routine is malformed when the return type is not implicitly castable to type T
/// </remarks>
template<class T, class Selector, class... Args>
struct has_well_formed_static_new {
  static const bool value = std::is_convertible<
    decltype(T::New(*(typename std::remove_reference<Args>::type*)nullptr...)),
    T*
  >::value;
};

template<class T, class... Args>
struct has_well_formed_static_new<T, std::false_type, Args...> {
  static const bool value = false;
};

template<typename T, typename... Args>
struct has_static_new
{
  template<class Fn, Fn>
  struct unnamed_constant;

  template<class U>
  static std::true_type select(decltype(U::New(*(typename std::remove_reference<Args>::type*)nullptr...))*);

  template<class U>
  static std::false_type select(...);

  static const bool value = has_well_formed_static_new<T, decltype(select<T>(nullptr)), Args...>::value;
};
