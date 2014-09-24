// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include "Deferred.h"

/// <summary>
/// Transmutes a function returning void to an instance that can be called by AutoPacket.
/// The specializations distinguish between void and Deferred return types.
/// </summary>
/// <remarks>
/// The default constructor yields an AutoFilter that does nothing.
/// </remarks>
template<class Ret, class... Args>
struct MicroAutoFilter {
  MicroAutoFilter(const std::function<void(Args...)>&) {}

  // This AutoFilter method will be identified as invalid due to the return type
  Ret AutoFilter(Args...) {}
};
template<class... Args>
struct MicroAutoFilter<void, Args...> {
  MicroAutoFilter(const std::function<void(Args...)>& filter):
    m_filter(filter)
  {}

  void AutoFilter(Args... args) {
    if (m_filter)
      return m_filter(std::forward<Args>(args)...);
  }

protected:
  std::function<void(Args...)> m_filter;
};

template<class... Args>
struct MicroAutoFilter<Deferred, Args...> {
  MicroAutoFilter(const std::function<void(Args...)>& filter):
    m_filter(filter)
  {}

  Deferred AutoFilter(Args... args) {
    if (m_filter)
      return m_filter(args...);
    return Deferred(this);
  }

protected:
  std::function<void(Args...)> m_filter;
};
