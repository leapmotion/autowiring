// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_arg.h"
#include <typeinfo>

/// <summary>
/// AutoFilter argument disposition
/// </summary>
struct AutoFilterArgument {
  AutoFilterArgument(void) = default;

  AutoFilterArgument(
    bool is_input,
    bool is_output,
    bool is_shared,
    bool is_multi,
    const std::type_info* ti,
    int tshift
  ) :
    is_input(is_input),
    is_output(is_output),
    is_shared(is_shared),
    is_multi(is_multi),
    ti(ti),
    tshift(tshift)
  {}

  const bool is_input = false;
  const bool is_output = false;
  const bool is_shared = false;
  const bool is_multi = false;
  const std::type_info* const ti = nullptr;
  const int tshift = 0;

  operator bool(void) const {
    return !!ti;
  }
};

template<typename T>
struct AutoFilterArgumentT:
  AutoFilterArgument
{
  AutoFilterArgumentT(void) :
    AutoFilterArgument(
      auto_arg<T>::is_input,
      auto_arg<T>::is_output,
      auto_arg<T>::is_shared,
      auto_arg<T>::is_multi,
      &typeid(typename auto_arg<T>::id_type),
      auto_arg<T>::tshift
    )
  {}
};