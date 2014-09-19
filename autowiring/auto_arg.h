// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <autowiring/auto_in.h>
#include <autowiring/auto_out.h>

/*
 The auto_arg<T> classes are used to generate of auto_in and auto_out_new types
 based on the argument type.
 The core functionality is that all argument types are treated as standard
 types, and can be re-derived from those standard types.
 Furthermore, by classifying arguments in terms of orientation and fundamental
 type, auto_arg enables the specification of a principal type to any class
 implementing an AutoFilter method.
 */

/// <summary>
/// Base case pertaining to unusable argument types in AutoFilter methods.
/// </summary>
/// <remarks>
/// The type nullptr_t can only be fullfilled by nullptr.
/// </remarks>
template<class type>
class auto_arg
{
  typedef std::nullptr_t auto_type;

  typedef std::nullptr_t id_type;
  typedef std::nullptr_t base_type;
  typedef std::shared_ptr<std::nullptr_t> shared_type;

  static const bool is_input = false;
  static const bool is_output = false;

  operator base_type () { return nullptr; }
  operator shared_type () { return std::shared_ptr<std::nullptr_t>(); }

  auto_arg(std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)) {}
};

/// <summary>
/// Specialization for "const T&" ~ auto_in<T>
/// </summary>
template<class type>
class auto_arg<const type&>:
  public auto_in<type>
{
public:
  typedef auto_in<type> auto_type;

  using typename auto_type::id_type;
  using typename auto_type::base_type;
  using typename auto_type::shared_type;

  using auto_type::is_input;
  using auto_type::is_output;

  using auto_type::operator typename auto_type::base_type;
  using auto_type::operator typename auto_type::shared_type;

  auto_arg(std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)):
    auto_in<type>(packet, source)
  {}
};

/// <summary>
/// Specialization for "std::shared_ptr<const T>" ~ auto_in<T>
/// </summary>
template<class type>
class auto_arg<std::shared_ptr<const type>>:
  public auto_in<type>
{
public:
  typedef auto_in<type> auto_type;

  using typename auto_type::id_type;
  using typename auto_type::base_type;
  using typename auto_type::shared_type;

  using auto_type::is_input;
  using auto_type::is_output;

  using auto_type::operator typename auto_type::base_type;
  using auto_type::operator typename auto_type::shared_type;

  auto_arg(std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)):
    auto_in<type>(packet, source)
  {}
};

/// <summary>
/// Specialization for equivalent type auto_in<T>
/// </summary>
template<class type>
class auto_arg<auto_in<type>>:
  public auto_in<type>
{
public:
  typedef auto_in<type> auto_type;

  using typename auto_type::id_type;
  using typename auto_type::base_type;
  using typename auto_type::shared_type;

  using auto_type::is_input;
  using auto_type::is_output;

  using auto_type::operator typename auto_type::base_type;
  using auto_type::operator typename auto_type::shared_type;

  auto_arg(std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)):
    auto_in<type>(packet, source)
  {}
};

/// <summary>
/// Specialization for "T&" ~ auto_in<T>
/// </summary>
template<class type>
class auto_arg<type&>:
  public auto_out_new<type>
{
public:
  typedef auto_out_new<type> auto_type;

  using typename auto_type::id_type;
  using typename auto_type::base_type;
  using typename auto_type::shared_type;

  using auto_type::is_input;
  using auto_type::is_output;

  using auto_type::operator typename auto_type::base_type;
  using auto_type::operator typename auto_type::shared_type;

  auto_arg(std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)):
    auto_out_new<type>(packet, source)
  {}
};

/// <summary>
/// Specialization for "std::unique_ptr<T, std::function<void(T*)>>" ~ auto_out_new<T>
/// </summary>
template<class type>
class auto_arg<std::unique_ptr<type, std::function<void(type*)>>>:
  public auto_out_new<type>
{
public:
  typedef auto_out_new<type> auto_type;

  using typename auto_type::id_type;
  using typename auto_type::base_type;
  using typename auto_type::shared_type;

  using auto_type::is_input;
  using auto_type::is_output;

  using auto_type::operator typename auto_type::base_type;
  using auto_type::operator typename auto_type::shared_type;

  auto_arg(std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)):
    auto_out_new<type>(packet, source)
  {}
};

/// <summary>
/// Specialization for equivalent type auto_out_new<T>
/// </summary>
template<class type>
class auto_arg<auto_out_new<type>>:
public auto_out_new<type>
{
public:
  typedef auto_out_new<type> auto_type;

  using typename auto_type::id_type;
  using typename auto_type::base_type;
  using typename auto_type::shared_type;

  using auto_type::is_input;
  using auto_type::is_output;

  using auto_type::operator typename auto_type::base_type;
  using auto_type::operator typename auto_type::shared_type;

  auto_arg(std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)):
    auto_out_new<type>(packet, source)
  {}
};

/*
class auto_pipe


class auto_actor
*/
