// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include "CoreContext.h"
#include "MicroAutoFilter.h"

/// <summary>
/// Wraps any method in a MicroAutoFilter class and registers it with the current context.
/// </summary>
/// <remarks>
/// In the constructor of a class that will be Injected into a context call this function as:
///  DeclareAutoFilter(this, &MyClass::MyMethod)
/// for each method that should be used as an AutoFilter.
/// </remarks>
template<class Base, class Ret, class... Args>
std::shared_ptr<MicroAutoFilter<Ret, Args...>> DeclareAutoFilter(Base* that, Ret (Base::*filter)(Args...)) {
  return std::shared_ptr<MicroAutoFilter<Ret, Args...>>(
    AutoCurrentContext()->template Construct<MicroAutoFilter<Ret, Args...>>(
      [that, filter] (Args... args) {
        return (that->*filter)(args...);
      }
    )
  );
}
