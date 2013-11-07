#pragma once
#include "AutoCheckout.h"

/// <summary>
/// A utility type to positively declare an output to an AutoFilter
/// </summary>
/// <param name="auto_ready">Set if the output should be marked "ready" by default</param>
/// <remarks>
/// The auto_output wrapper is a convenience alternative to accepting an AutoPacket
/// and then requesting a checkout.  It provides the additional benefit that the
/// caller may declaratively specify whether the auto_output should be satisfied by
/// default when the function returns, or if it must be manually satisfied with a
/// direct call to Ready before the object falls out of scope
/// </remarks>
template<class T, bool auto_ready = true>
class auto_out {
public:
  auto_out(AutoCheckout<T>&& checkout) :
    m_checkout(std::move(checkout)),
    m_cancelled(false)
  {}

  ~auto_out(void) {
    if(!m_cancelled)
      m_checkout.Ready();
  }

private:
  bool m_cancelled;
  AutoCheckout<T> m_checkout;

public:
  void Cancel(void) const { m_cancelled = true; }
  T* operator->(void) const { return m_checkout; }
  operator T*(void) const { return m_checkout; }
};

// The non-autoready specialization is just a checkout alias:
template<class T>
class auto_out<T, false>:
  public AutoCheckout<T>
{
public:
  auto_out(AutoCheckout<T>&& checkout):
    AutoCheckout<T>(std::move(checkout))
  {}
};