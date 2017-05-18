// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include TYPE_TRAITS_HEADER
#include RVALUE_HEADER

namespace autowiring {

template <class T, typename... Args>
class has_static_new
{
  template<class U, class = typename std::enable_if<
    std::is_function<decltype(U::New(std::declval<Args>()...))(Args...)>::value &&
    std::is_convertible<decltype(U::New(std::declval<Args>()...)), T*>::value
  >::type>
  static std::true_type check(void*);
  template <class...>
  static std::false_type check(...);
public:
  static const bool value = decltype(check<T>(nullptr))::value;
};

}
