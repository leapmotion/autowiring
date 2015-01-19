// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoPacket.h"

/// <summary>
/// Identifier for AutoFilter inputs from the previous packet
/// </summary>
/// <remarks>
/// When auto_prev is used as an AutoFilter argument, it's value will the value of 'T'
/// from the previous packet. It's null for the the first packet
/// </remarks>
template<class T, int N = 1>
struct auto_prev {
public:
  auto_prev(const AutoPacket& val)
  {
    val.Get(m_value, N);
  }

  operator bool(void) const {
    return m_value.operator bool();
  }

  const T& operator*(void) const {
    return *m_value;
  }
  
  const T* operator->(void) const {
    return m_value.get();
  }

  std::shared_ptr<const T> m_value;
};
