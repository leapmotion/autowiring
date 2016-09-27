// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_arg.h"

namespace autowiring {

template<class T>
class auto_arg;

/// <summary>
/// Identifier for AutoFilter inputs from the previous packet
/// </summary>
/// <remarks>
/// When auto_prev is used as an AutoFilter argument, its value will be the value of 'T'
/// from the previous packet. It's null for the the first packet
/// </remarks>
template<class T, int N = 1>
struct auto_prev {
public:
  auto_prev(const T* value) :
    value(value)
  {}

  operator bool(void) const {
    return !!value;
  }

  const T& operator*(void) const {
    return *value;
  }

  const T* operator->(void) const {
    return value;
  }

  const T* const value;
};

template<class T, int N>
class auto_arg<auto_prev<T, N>>
{
public:
  typedef auto_prev<T, N> type;
  typedef auto_prev<T, N> arg_type;
  typedef auto_id_t<T> id_type;

  static const bool is_input = true;
  static const bool is_output = false;
  static const bool is_rvalue = false;
  static const bool is_shared = true;
  static const bool is_multi = false;
  static const int tshift = N;

  template<class C>
  static const T* arg(C& packet) {
    const T* retVal;
    packet.template Get<T>(retVal, N);
    return retVal;
  }
};

}
