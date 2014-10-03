// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include "DeclareElseFilter.h"
#include "SatCounter.h"

/// <summary>If Base::AutoFilter is not called, this will execute a Final-Call method</summary>
template<class>
class MicroElseFilter
{
protected:
  const std::type_info& type;
  std::function<void(const AutoPacket&)> m_filter;

public:
  MicroElseFilter(const std::type_info& type, const std::function<void(const AutoPacket&)>& filter):
    type(type),
    m_filter(filter)
  {}

  void AutoFilter(const AutoPacket& packet) {
    if(packet.GetSatisfaction(type).called) {
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
  typedef typename SelectTypeUnifier<Base>::type t_repType;

  return AutoCurrentContext()->template Construct<MicroElseFilter<Base>>(
    typeid(t_repType),
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
    void (Base::*filter)(const AutoPacket&)
  )
{
  // If this type will use a unifier, we need to declare the else filter on the unifier type,
  // not the represented type, because the unifier type will be the actual true type of the
  // class in the context.
  // TODO:  The fact that this is necessary implies that the concept of an else-filter based
  // on whether an AutoFilter declared on a particular type may be ill-formed; it might be
  // necessary to revisit this concept and instead declare a contingent filter piecewise in
  // terms of its input arguments rather than in terms of another filter entry.
  typedef typename SelectTypeUnifier<Base>::type t_repType;
  return AutoCurrentContext()->template Construct<MicroElseFilter<Next>>(
    typeid(t_repType),
    [that, filter] (const AutoPacket& packet) {
      return (that->*filter)(packet);
    }
  );
}
