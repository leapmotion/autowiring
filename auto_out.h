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
    m_checkout(std::move(checkout))
  {
    // Mark ready by default:
    m_checkout.Ready();
  }

  ~auto_out(void) {}

private:
  AutoCheckout<T> m_checkout;

public:
  T* operator->(void) const { return m_checkout; }
  operator T*(void) const { return m_checkout; }
};