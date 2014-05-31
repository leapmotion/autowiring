#pragma once

class AutoPacket;

/// <summary>
/// A single custom AutoFilter entry in some type
/// </summary>
class AutoFilterBase {
public:
  typedef void(*t_call)(void*, AutoPacket&);

protected:
  AutoFilterBase(t_call pCall);

public:
  // The actual call held by this filter entry
  const t_call pCall;

  // The next filter AutoFilter entry in the series
  AutoFilterBase* pFlink;
};

template<class MemFn, MemFn memFn>
class AutoFilter:
  public AutoFilterBase
{
public:
  AutoFilter(void) :
    AutoFilterBase(&CallExtractor<MemFn>::Call<memFn>)
  {}
};