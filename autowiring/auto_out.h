// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include MEMORY_HEADER
#include <autowiring/AutoPacket.h>

/// <summary>
/// Declares an output of an AutoFilter method via an argument.
/// </summary>
/// <remarks>
/// The output of the argument arg can be prevented by calling
///  delete arg.release();
/// or by redefining the deleter as follows:
///  arg.get_deleter() = &std::default_delete<type>();
/// Capturing shared_ptr<AutoPacket> in the destructor lambda
/// ensures that the lifespan of the AutoPacket will exceed
/// the lifespan of the auto_out instance.
/// </remarks>
template <class type>
class auto_out_new:
  public std::unique_ptr<type, std::function<void(type*)>>
{
public:
  typedef type id_type;
  typedef const type& base_type;
  typedef std::unique_ptr<type, std::function<void(type*)>> shared_type;

  static const bool is_input = false;
  static const bool is_output = true;

  operator base_type () const {
    return *(this->get());
  }

  operator shared_type () {
    return *this;
  }

  auto_out_new ():
    shared_type(nullptr)
  {}


  /// <summary>Copy constructor is equivalent to a move</summary>
  auto_out_new (auto_out_new<type>& rhs) {
    std::swap<shared_type>(*this, rhs);
  }

  auto_out_new (auto_out_new<type>&& rhs) {
    std::swap<shared_type>(*this, rhs);
  }

  /// <summary>Assignment from is equivalent to a move</summary>
  /// <remarks>
  /// If the lhs auto_out has a non-null reference that reference
  /// will be finalized before assignment.
  /// </remarks>
  auto_out_new& operator = (auto_out_new<type>& rhs) {
    shared_type::reset();
    std::swap<shared_type>(*this, rhs);
    return *this;
  }

  auto_out_new& operator = (auto_out_new<type>&& rhs) {
    shared_type::reset();
    std::swap<shared_type>(*this, rhs);
    return *this;
  }

  auto_out_new (std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)):
    shared_type(new type(), std::function<void(type*)>([packet, &source](type* ptr){
      // NOTE: This is only called when ptr is valid
      packet->Put(ptr, source);
    }))
  {}
};

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