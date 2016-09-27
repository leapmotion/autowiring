// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include TYPE_TRAITS_HEADER

namespace autowiring {

template<typename T, bool isAbstract = std::is_abstract<T>::value>
struct has_simple_constructor
{
  static const bool value = std::is_constructible<T>::value;
};

template<typename T>
struct has_simple_constructor<T, true>
{
  static const bool value = false;
};

}
