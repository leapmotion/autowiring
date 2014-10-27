// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoFilterDescriptor.h"
#include "autowiring.h"

/// <summary>
/// A single custom AutoFilter entry in some type
/// </summary>
template<class MemFn>
class NewAutoFilterForwarder;

template<class RetType, class T, class... Args>
class NewAutoFilterForwarder<RetType(T::*)(Args...)>
{
public:
  typedef RetType(T::*t_memfn)(Args...);

  NewAutoFilterForwarder(T* p, t_memfn pfn) :
    p(p),
    pfn(pfn)
  {}

private:
  T* const p;
  const t_memfn pfn;

public:
  void AutoFilter(Args... args) {
    (p->*pfn)(std::forward<Args>(args)...);
  }
};

/// <summary>
/// Constructs a relay for a secondary AutoFilter function
/// </summary>
class NewAutoFilter
{
public:
  template<class RetVal, class T, class... Args>
  NewAutoFilter(T* ptr, RetVal (T::*memfn)(Args...)) {
    AutoConstruct<NewAutoFilterForwarder<RetVal(T::*)(Args...)>> ar(ptr, memfn);
  }
};