#pragma once
#include "AutoFilterDescriptor.h"

class AutoPacket;

/// <summary>
/// A single custom AutoFilter entry in some type
/// </summary>
class AutoFilterBase {
protected:
  AutoFilterBase(const AutoFilterDescriptorStub& stub);

public:
  // The actual descriptor stub
  const AutoFilterDescriptorStub& m_stub;

  // The next filter AutoFilter entry in the series
  AutoFilterBase* pFlink;
};

template<class MemFn, MemFn memFn>
class AutoFilter:
  public AutoFilterBase
{
public:
  static const AutoFilterDescriptorStub& GetStub(void) {
    static const AutoFilterDescriptorStub s_descriptor(CallExtractor<MemFn>(), &CallExtractor<MemFn>::Call<memFn>);
    return s_descriptor;
  }

  AutoFilter(void) :
    AutoFilterBase(GetStub())
  {}
};
