// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "ContextMember.h"
#include "DeclareElseFilter.h"
#include "SatCounter.h"

template<class Base, class... Args>
class MicroElseFilter;

/// <summary>
/// Zero-argument specialization
/// </summary>
template<class Base>
class MicroElseFilter<Base> :
  public ContextMember
{
public:
  MicroElseFilter(Base* base, void (Base::*filter)(const AutoPacket&)) :
    ContextMember("MicroElseFilter"),
    base(base),
    filter(filter)
  {}

protected:
  Base* base;
  void (Base::*filter)(const AutoPacket&);
};

/// <summary>If Base::AutoFilter is not called, this will execute a Final-Call method</summary>
template<class Base, class... Args>
class MicroElseFilter:
  public MicroElseFilter<Base>
{
public:
  MicroElseFilter(Base* base, void (Base::*filter)(const AutoPacket&)) :
    MicroElseFilter<Base>(base, filter)
  {}

  void AutoFilter(const AutoPacket& packet) {
    const bool has_all[] = {packet.Has<Args>()... };

    for(bool cur : has_all)
      if(!cur) {
        // Missing decoration, base filter wasn't called
        (this->base->*filter)(packet);
        return;
      }

    /// Base::AutoFilter has already been called for this packet
  }
};

/// <summary>
/// Creates a MicroElseFilter to be called when the specified AutoFilter routine is not called
/// </summary>
/// <param name="testFilter">The filter whose state is to be tested when determining whether to call</param>
template<class Test, class Base, class... Args>
std::shared_ptr<MicroElseFilter<Base>> DeclareElseFilter(void(Test::*testFilter)(Args...), Base* that, void (Base::*filter)(const AutoPacket&)) {
  AutoCurrentContext ctxt;
  return ctxt->template Inject<MicroElseFilter<Base, Args...>>(that, filter);
}

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
  return DeclareElseFilter(&Base::AutoFilter, that, filter);
}
