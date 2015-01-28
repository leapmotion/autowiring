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

  template<class T>
  AutoFilterDescriptorInput(auto_arg<T>*) :
    is_input(auto_arg<T>::is_input),
    is_output(auto_arg<T>::is_output),
    is_shared(auto_arg<T>::is_shared),
    is_multi(auto_arg<T>::is_multi),
    ti(&typeid(typename auto_arg<T>::id_type)),
    tshift(auto_arg<T>::tshift)
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

  template<class T>
  struct rebind {
    operator AutoFilterDescriptorInput() {
      return AutoFilterDescriptorInput((auto_arg<T>*)nullptr);
    }
  };
};
