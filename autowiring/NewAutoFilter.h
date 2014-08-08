// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoFilterDescriptor.h"

/// <summary>
/// A single custom AutoFilter entry in some type
/// </summary>
class NewAutoFilterBase {
protected:
  NewAutoFilterBase(const AutoFilterDescriptorStub& stub);
};

template<class MemFn, MemFn memFn>
class NewAutoFilter:
  public NewAutoFilterBase
{
public:
  static const AutoFilterDescriptorStub& GetStub(void) {
    static const AutoFilterDescriptorStub s_descriptor(CallExtractor<MemFn>(), &CallExtractor<MemFn>::template Call<memFn>);
    return s_descriptor;
  }

  NewAutoFilter(void) :
    NewAutoFilterBase(GetStub())
  {}
};
