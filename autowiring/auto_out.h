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
class auto_out:
  public std::unique_ptr<type, std::function<void(type*)>>
{
public:
  typedef type id_type;
  typedef type& base_type;
  typedef std::unique_ptr<type, std::function<void(type*)>> shared_type;

  static const bool is_input = false;
  static const bool is_output = true;

  operator base_type () const {
    return *(this->get());
  }

  operator shared_type () {
    return *this;
  }

  auto_out ():
    shared_type(nullptr)
  {}


  /// <summary>Copy constructor is equivalent to a move</summary>
  auto_out (auto_out<type>& rhs) {
    std::swap<shared_type>(*this, rhs);
  }

  auto_out (auto_out<type>&& rhs) {
    std::swap<shared_type>(*this, rhs);
  }

  /// <summary>Assignment from is equivalent to a move</summary>
  /// <remarks>
  /// If the lhs auto_out has a non-null reference that reference
  /// will be finalized before assignment.
  /// </remarks>
  auto_out& operator = (auto_out<type>& rhs) {
    shared_type::reset();
    std::swap<shared_type>(*this, rhs);
    return *this;
  }

  auto_out& operator = (auto_out<type>&& rhs) {
    shared_type::reset();
    std::swap<shared_type>(*this, rhs);
    return *this;
  }

  auto_out (std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)):
    shared_type(new type(), std::function<void(type*)>([packet, &source](type* ptr){
      // NOTE: This is only called when ptr is valid
      packet->Put(ptr, source);
    }))
  {}
};
