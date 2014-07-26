#pragma once
#include <autowiring/CoreThread.h>
#include <autowiring/has_autofilter.h>

/// <summary>
/// A simple "decoration" class which will be added to a variety of sample packets
/// </summary>
template<int N>
class Decoration {
public:
  Decoration(int val = N) :
    i(val)
  {}

  int i;
};

// Some simple filters which use the above decoration:
class FilterRoot {
public:
  FilterRoot(void) :
    m_called(0)
  {}

  int m_called;
  Decoration<0> m_zero;
  Decoration<1> m_one;
};

class FilterA:
  public FilterRoot,
  public ContextMember
{
public:
  FilterA() {}
  virtual ~FilterA() {}
  void AutoFilter(Decoration<0> zero, Decoration<1> one) {
    ++m_called;
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
    m_excepted(false)
  {}

  Deferred AutoFilter(Decoration<0> zero, Decoration<1> one) {
    ++m_called;
    m_zero = zero;
    m_one = one;
    return Deferred(this);
  }

  bool m_proceed;
  bool m_excepted;

  void Continue(void) {
    PerformStatusUpdate([this] { m_proceed = true; });
  }

  void Run(void) override {
    WaitForStateUpdate([this] { return m_proceed; });

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
    ++m_called;
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
    ++m_called;
    pkt.Decorate(Decoration<2>());
  }
};

/// <summary>
/// An override of the FilterD AutoFilter method
/// </summary>
class FilterE:
public FilterD
{
public:
  void AutoFilter(AutoPacket& pkt) {
    ++m_called;
    pkt.Decorate(Decoration<3>());
  }
};

class FilterF:
  public FilterRoot,
  public ContextMember
{
public:
  void AutoFilter(const Decoration<0>& dec) {
    ++m_called;
  }
};

/// <summary>
/// A generic filter which accepts templated input types
/// </summary>
template<class... Args>
class FilterGen {
public:
  FilterGen(void):
    m_called(0)
  {}

  void AutoFilter(AutoPacket& packet, Args... args) {
    // Detect cases where a non-const reference is already decorated on the input packet
    bool detection [] = {
      !std::is_const<typename std::remove_reference<Args>::type>::value &&
      std::is_reference<Args>::value &&
      packet.Has<Args>()...
    };

    for(bool cur : detection)
      ASSERT_FALSE(cur) << "Packet was already decorated with at least one output-only type";

    ++m_called;
    m_args = std::tie(args...);
  }

  int m_called;
  std::tuple<typename std::decay<Args>::type...> m_args;
};

/// <summary>
/// A filter that should trigger a static_assert in AutoRequire<BadFilterA>
/// </summary>
class BadFilterA:
public FilterRoot
{
public:
  void AutoFilter(void) {
    ++m_called;
  }
};

/// <summary>
/// A filter that should trigger a static_assert in AutoRequire<BadFilterB>
/// </summary>
class BadFilterB:
public FilterRoot
{
public:
  void AutoFilter(Decoration<0>&) {
    ++m_called;
  }
  void AutoFilter(Decoration<1>&) {
    ++m_called;
  }
};

/// <summary>
/// Automatically obtains and returns a modified Decoration<0>
/// Appemnds a decoration to pkt.
/// </summary>
class FilterOutA :
  public FilterRoot {
public:
    void AutoFilter(AutoPacket& pkt, auto_out<Decoration<0>> zero) {
      ++m_called;
      pkt.Decorate(Decoration<1>());
      ++zero->i;
    }
};

/// <summary>
/// Automatically obtains and returns a modified Decoration<0>
/// Does not reference AutoPacket directly.
/// </summary>
class FilterOutB :
public FilterRoot {
public:
  void AutoFilter(auto_out<Decoration<2>>) {
    ++m_called;
  }
};
