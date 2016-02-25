// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "ContextMember.h"
#include "Deferred.h"
#include FUNCTIONAL_HEADER

/// <summary>
/// Transmutes a function returning void to an instance that can be called by AutoPacket.
/// The specializations distinguish between void and Deferred return types.
/// </summary>
/// <remarks>
/// The default constructor yields an AutoFilter that does nothing.
/// </remarks>
template<class Ret, class... Args>
struct MicroAutoFilter:
  public ContextMember
{
  MicroAutoFilter(const std::function<void(Args...)>&&):
    ContextMember("MicroAutoFilter")
  {}

  // This AutoFilter method will be identified as invalid due to the return type
  Ret AutoFilter(Args...) {}
};
template<class... Args>
struct MicroAutoFilter<void, Args...>:
  public ContextMember
{
  MicroAutoFilter(const std::function<void(Args...)>&& filter):
    ContextMember("MicroAutoFilter"),
    m_filter(std::move(filter))
  {}

  void AutoFilter(Args... args) {
    if (m_filter)
      return m_filter(std::forward<Args>(args)...);
  }

protected:
  std::function<void(Args...)> m_filter;
};

template<class... Args>
struct MicroAutoFilter<Deferred, Args...> :
  public ContextMember
{
  MicroAutoFilter(const std::function<void(Args...)>&& filter):
    ContextMember("MicroAutoFilter"),
    m_filter(std::move(filter))
  {}

  Deferred AutoFilter(Args... args) {
    if (m_filter)
      return m_filter(args...);
    return Deferred(this);
  }

protected:
  std::function<void(Args...)> m_filter;
};
