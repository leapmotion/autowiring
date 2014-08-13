// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include "is_auto_filter.h"
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
  // This case pertains only when the return value is not recognized
  static_assert(is_auto_filter_return<Ret>{},
                "The return is not an allowed type for AutoFilter methods");
};
template<class... Args>
struct MicroAutoFilter<void, Args...> {
  MicroAutoFilter(const std::function<void(Args...)>& filter) : m_filter(filter) {
    static_assert(all_auto_filter_args<Args...>::value,
                  "At least one argument is not an allowed type for AutoFilter methods");
  }

  void AutoFilter(Args... args) {
    if (m_filter)
      return m_filter(std::move(args)...);
  }

protected:
  std::function<void(Args...)> m_filter;
};
template<class... Args>
struct MicroAutoFilter<Deferred, Args...> {
  MicroAutoFilter(const std::function<void(Args...)>& filter) : m_filter(filter) {
    static_assert(all_auto_filter_args<Args...>::value,
                  "At least one argument is not an allowed type for AutoFilter methods");
  }

  Deferred AutoFilter(Args... args) {
    if (m_filter)
      return m_filter(std::move(args)...);
    return Deferred(this);
  }

protected:
  std::function<void(Args...)> m_filter;
};
