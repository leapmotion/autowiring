// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

/// <summary>
/// A wrapper type, used to indicate a pointer which may optionally be satisfied
/// </summary>
/// <remarks>
/// An optional pointer denotes a field which may be omitted during a call to a filter
/// method.  The caller is responsible for testing a field for correctness.
///
/// Consumers are cautioned against using optional_ptr where not necessary.  Users of
/// optional_ptr will only be notified that a field is not satisfied when the packet is
/// about to be destroyed, or the field has been explicitly marked as unsatisfiable
/// (for example, due to a failed checkout).
/// </remarks>
template <class type>
class optional_ptr:
  public std::shared_ptr<const type>
{
public:
  typedef type id_type;
  typedef const type* base_type;
  typedef std::shared_ptr<const type> shared_type;
  // WARNING: optional_ptr<T>::shared_type == auto_in<T>::shared_type

  static const bool is_input = true;
  static const bool is_output = false;

  operator base_type () const {
    return this->get();
  }

  operator shared_type () {
    return *this;
  }

  optional_ptr ():
    shared_type(nullptr)
  {}

  optional_ptr (optional_ptr<type>&& rhs):
    std::shared_ptr<const type>(std::move(rhs))
  {}

  optional_ptr(std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)) {
    packet->Get(*this, source);
  }

  optional_ptr& operator = (optional_ptr<type>&& rhs) {
    *this = std::forward<std::shared_ptr<const type>&&>(rhs);
    return *this;
  }
};
