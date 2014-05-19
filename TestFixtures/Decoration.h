#pragma once
#include "../CoreThread.h"
#include <boost/thread/barrier.hpp>

/// <summary>
/// A simple "decoration" class which will be added to a variety of sample packets
/// </summary>
template<int N>
class Decoration {
public:
  Decoration(void) :
    i(N)
  {}

  int i;
};

// Some simple filters which use the above decoration:
class FilterRoot {
public:
  FilterRoot(void) :
    m_called(false)
  {}

  bool m_called;
  Decoration<0> m_zero;
  Decoration<1> m_one;
};

class FilterA:
  public FilterRoot,
  public virtual ContextMember
{
public:
  FilterA() {}
  virtual ~FilterA() {}
  void AutoFilter(Decoration<0> zero, Decoration<1> one) {
    m_called = true;
    m_zero = zero;
    m_one = one;
  }
};
static_assert(has_autofilter<FilterA>::value, "Expected the filter to have an AutoFilter method");

class FilterB:
  public CoreThread,
  public FilterRoot
{
public:
  FilterB(void) :
    m_barr(2),
    m_excepted(false)
  {}

  Deferred AutoFilter(Decoration<0> zero, Decoration<1> one) {
    m_called = true;
    m_zero = zero;
    m_one = one;
    return Deferred(this);
  }

  boost::barrier m_barr;
  bool m_excepted;

  void Run(void) override {
    m_barr.wait();
    try {
      CoreThread::Run();
    } catch(dispatch_aborted_exception&) {
      // Expected exception type
      throw;
    } catch(...) {
      m_excepted = true;
    }
  }
};

/// <summary>
/// This is a filter which, in addition to accepting a decoration, also accepts a packet and tries to decorate it
/// </summary>
class FilterC:
  public FilterRoot
{
public:
  void AutoFilter(AutoPacket& pkt, const Decoration<0>& zero) {
    // Copy out:
    m_called = true;
    m_zero = zero;

    // Add a decoration:
    pkt.Decorate(Decoration<1>());
  }
};

/// <summary>
/// A filter which will simply get hit any time a packet is issued in the current context
/// </summary>
class FilterD:
  public FilterRoot
{
public:
  void AutoFilter(AutoPacket& pkt) {
    m_called = true;
  }
};

class FilterF:
  public FilterRoot,
  public virtual ContextMember
{
public:
  void AutoFilter(const Decoration<0>& dec) {
    m_called = true;
  }
};