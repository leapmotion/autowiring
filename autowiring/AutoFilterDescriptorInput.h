// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_arg.h"
#include <typeinfo>

/// <summary>
/// AutoFilter argument disposition
/// </summary>
struct AutoFilterDescriptorInput {
  AutoFilterDescriptorInput(void) :
    is_input(false),
    is_output(false),
    is_shared(false),
    is_multi(false),
    ti(nullptr),
    tshift(0)
  {}

  AutoFilterDescriptorInput(
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

  const bool is_input;
  const bool is_output;
  const bool is_shared;
  const bool is_multi;
  const std::type_info* const ti;
  const int tshift;

  operator bool(void) const {
    return !!ti;
  }
};

template<typename T>
struct AutoFilterDescriptorInputT:
  AutoFilterDescriptorInput
{
  AutoFilterDescriptorInputT(void) :
    AutoFilterDescriptorInput(
      auto_arg<T>::is_input,
      auto_arg<T>::is_output,
      auto_arg<T>::is_shared,
      auto_arg<T>::is_multi,
      &typeid(typename auto_arg<T>::id_type),
      auto_arg<T>::tshift
    )
  {}
};