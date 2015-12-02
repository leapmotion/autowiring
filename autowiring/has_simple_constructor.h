// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include TYPE_TRAITS_HEADER

namespace autowiring {

template<typename T, bool isAbstract = std::is_abstract<T>::value>
struct has_simple_constructor
{
  template<class U>
  static int select(decltype(U())*);

  template<class U>
  static char select(...);

  static const bool value = sizeof(select<T>(nullptr)) == sizeof(int);
};

template<typename T>
struct has_simple_constructor<T, true>
{
  static const bool value = false;
};

}
