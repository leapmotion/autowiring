// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once

template<typename... >
using void_t = void;

template<typename T, typename = void>
struct is_input_iterator:
  std::false_type
{};

template<typename T>
struct is_input_iterator<T,
  void_t<decltype(++std::declval<T>()),
         decltype(*std::declval<T>()),
         decltype(std::declval<T>() != std::declval<T>())
        >> :
  std::true_type
{};
