// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include MEMORY_HEADER
#include <autowiring/AutoPacket.h>

/// <summary>
/// Fundamental type of required input arguments of AutoFilter methods.
/// </summary>
/// <remarks>
/// Argument types that resolve to and from auto_in are:
///  const T&
///  std::shared_ptr<const T>
/// </remarks>
template <class type>
class auto_in:
  public std::shared_ptr<const type>
{
public:
  typedef type id_type;
  typedef const type& base_type;
  typedef std::shared_ptr<const type> shared_type;

  static const bool is_input = true;
  static const bool is_output = false;

  operator base_type () const {
    return *(this->get());
  }

  operator shared_type () {
    return *this;
  }

  auto_in ():
    shared_type(nullptr)
  {}

  auto_in (auto_in<type>& rhs):
    shared_type(rhs)
  {}

  auto_in (auto_in<type>&& rhs) {
    std::swap<shared_type>(*this, rhs);
  }

  auto_in& operator = (auto_in<type>& rhs) {
    shared_type::operator = (rhs);
    return *this;
  }

  auto_in& operator = (auto_in<type>&& rhs) {
    shared_type::reset();
    std::swap<shared_type>(*this, rhs);
    return *this;
  }

  auto_in (std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)) {
    packet->Get(*this, source);
  }
};
