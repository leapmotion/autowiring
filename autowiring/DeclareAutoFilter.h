// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include "CoreContext.h"
#include "MicroAutoFilter.h"

/// <summary>Child of MicroAutoFilter that is additionally distinguished by base type</summary>
template<class Base, class Ret, class... Args>
class BasedAutoFilter
{
protected:
  MicroAutoFilter<Ret, Args...> m_micro;

public:
  BasedAutoFilter(const std::function<void(Args...)>&& filter) : m_micro(std::move(filter)) {}
  Ret AutoFilter(Args... args) { return m_micro.AutoFilter(args...); }
};

/// <summary>
/// Wraps any method in a MicroAutoFilter class and registers it with the current context.
/// </summary>
/// <remarks>
/// In the constructor of a class that will be Injected into a context call this function as:
///  DeclareAutoFilter(this, &MyClass::MyMethod)
/// for each method that should be used as an AutoFilter.
/// </remarks>
template<class Base, class Ret, class... Args>
std::shared_ptr<BasedAutoFilter<Base, Ret, Args...>> DeclareAutoFilter(Base* that, Ret (Base::*filter)(Args...)) {
  return AutoCurrentContext()->template Construct<BasedAutoFilter<Base, Ret, Args...>>(
    [that, filter] (Args... args) {
      return (that->*filter)(args...);
    }
  );
}
