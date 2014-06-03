#pragma once
#include "AutoFilterDescriptor.h"

/// <summary>
/// A single custom AutoFilter entry in some type
/// </summary>
class NewAutoFilterBase {
protected:
  NewAutoFilterBase(const AutoFilterDescriptorStub& stub);

public:
  // The actual descriptor stub
  const AutoFilterDescriptorStub& m_stub;

  // The next filter AutoFilter entry in the series
  NewAutoFilterBase* pFlink;
};

template<class MemFn, MemFn memFn>
class NewAutoFilter:
  public NewAutoFilterBase
{
public:
  static const AutoFilterDescriptorStub& GetStub(void) {
    static const AutoFilterDescriptorStub s_descriptor(CallExtractor<MemFn>(), &CallExtractor<MemFn>::Call<memFn>);
    return s_descriptor;
  }

  NewAutoFilter(void) :
    AutoFilterBase(GetStub())
  {}
};
