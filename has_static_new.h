#pragma once
#include TYPE_TRAITS_HEADER

template<typename T, typename... Args>
struct has_static_new
{
  template<class Fn, Fn>
  struct unnamed_constant;

  template<class U>
  static int select(decltype(U::New(*(typename std::remove_reference<Args>::type*)nullptr...))*);

  template<class U>
  static char select(...);

  static const bool value = sizeof(select<T>(nullptr)) == sizeof(int);
};