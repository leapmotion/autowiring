// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

/// <summary>
/// Identifier for AutoFilter inputs from the previous packet
/// </summary>
/// <remarks>
/// When auto_prev is used as an AutoFilter argument, it's value will the value of 'T'
/// from the previous packet. It's null for the the first packet
/// </remarks>
template<class T>
struct auto_prev {
  auto_prev(const std::shared_ptr<T>& val):
    m_value(val)
  {}

  operator bool(void) const {
    return bool(m_value);
  }

  const T& operator*(void) const {
    return *m_value;
  }

  std::shared_ptr<T> m_value;
};
