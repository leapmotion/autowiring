// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include "DeclareElseFilter.h"
#include "SatCounter.h"

/// <summary>If Base::AutoFilter is not called, this will execute a Final-Call method</summary>
template<class Base>
class MicroElseFilter
{
protected:
  std::function<void(const AutoPacket&)> m_filter;

public:
  MicroElseFilter(const std::function<void(const AutoPacket&)>& filter) : m_filter(filter) {}

  void AutoFilter(const AutoPacket& packet) {
    if (packet.GetSatisfaction(typeid(Base)).called) {
      /// Base::AutoFilter has already been called for this packet
      return;
    }
    m_filter(packet);
  }
};

/// <summary>
/// Creates a MicroElseFilter class that will call the method provided in the constructor
/// if and only if Base::AutoFilter has not been called.
/// </summary>
/// <remarks>
/// In the constructor of a class that will be Injected into a context call this function as:
///  DeclareElseFilter(this, &MyClass::ElseFilter)
/// </remarks>
template<class Base>
std::shared_ptr<MicroElseFilter<Base>> DeclareElseFilter(Base* that, void (Base::*filter)(const AutoPacket&)) {
  return AutoCurrentContext()->template Construct<MicroElseFilter<Base>>(
    [that, filter] (const AutoPacket& packet) {
      return (that->*filter)(packet);
    }
  );
}

/// <summary>
/// Specialization for the case where the AutoFilter method is registered using DeclareAutoFilter
/// </summary>
/// <remarks>
/// In the constructor of a class that will be Injected into a context call this function as:
///  DeclareElseFilter<Base, Next>(this, &MyClass::ElseNextFilter)
/// where m_NextFilter is a shared pointer to the BasedAutoFilter instance.
/// </remarks>
template<class Base, class Next>
std::shared_ptr<MicroElseFilter<Next>> DeclareElseFilter(
  Base* that,
  void (Base::*filter)(const AutoPacket&)) {
  return AutoCurrentContext()->template Construct<MicroElseFilter<Next>>(
    [that, filter] (const AutoPacket& packet) {
      return (that->*filter)(packet);
    }
  );
}
