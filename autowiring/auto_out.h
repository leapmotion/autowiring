// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoCheckout.h"
#include RVALUE_HEADER
#include <algorithm>

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
  /// <remarks>
  /// Copy constructor is a move.
  /// This enables compliance with the expected non-reference argument type auto_out<T>.
  ///</remarks>
  auto_out(const auto_out& rhs) :
    m_cancelled(false),
    m_checkout(std::move(rhs.m_checkout))
  {
    // Ensure we do not try to multiply commit this checkout:
    rhs.m_cancelled = true;
  }

  auto_out(auto_out&& rhs) :
  m_cancelled(false),
  m_checkout(std::move(rhs.m_checkout))
  {
    // Ensure we do not try to multiply commit this checkout:
    rhs.m_cancelled = true;
  }

  explicit auto_out(AutoCheckout<T>&& checkout) :
    m_cancelled(false),
    m_checkout(std::move(checkout))
  {}

  auto_out(void):
    m_cancelled(false)
  {}
  ~auto_out(void) {
    if(!m_cancelled)
      m_checkout.Ready();
  }

private:
  mutable bool m_cancelled;
  mutable AutoCheckout<T> m_checkout;

public:
  //<summary>Mimics interface of auto_out<T, false> </summary>
  void Ready() const {}

  T* ptr(void) const { return m_checkout; }

  void Cancel(void) const { m_cancelled = true; }
  T* operator->(void) const { return m_checkout; }
  operator T*(void) const { return m_checkout; }
  operator T&(void) const { return m_checkout; }
  T& operator*(void) const { return m_checkout; }
};

// The non-autoready specialization is just a checkout alias:
template<class T>
class auto_out<T, false>:
  public AutoCheckout<T>
{
public:
  /// <remarks>
  /// Copy constructor is a move.
  /// This enables compliance with the expected non-reference argument type auto_out<T, false>.
  /// </remarks>
  auto_out(auto_out& rhs):
    AutoCheckout<T>(std::move(rhs))
  {}

  auto_out(auto_out&& rhs):
    AutoCheckout<T>(std::move(rhs))
  {}

  explicit auto_out(AutoCheckout<T>&& checkout) :
    AutoCheckout<T>(std::move(checkout))
  {}
};