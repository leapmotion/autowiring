// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <autowiring/auto_in.h>
#include <autowiring/auto_out.h>

/*
 The auto_arg<T> classes are used to generate of auto_in and auto_out types
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
  typedef std::nullptr_t index_type;

  typedef std::nullptr_t arg_type;
  typedef std::nullptr_t base_type;
  typedef std::shared_ptr<std::nullptr_t> shared_type;

  static const bool is_input = false;
  static const bool is_output = false;

  operator base_type () { return nullptr; }
  operator shared_type () { return std::shared_ptr<std::nullptr_t>(); }
};

/// <summary>
/// Specialization for "const T&" ~ auto_in<T>
/// </summary>
template<class type>
class auto_arg<const type&>:
  public auto_in<type>
{
public:
  typedef auto_in<type> index_type;

  using typename index_type::arg_type;
  using typename index_type::base_type;
  using typename index_type::shared_type;

  using index_type::is_input;
  using index_type::is_output;

  using index_type::operator typename index_type::base_type;
  using index_type::operator typename index_type::shared_type;

  auto_arg(std::shared_ptr<AutoPacket> packet):
    auto_in<type>(packet)
  {}
};

template<class type>
class auto_arg<std::shared_ptr<const type>>:
  public auto_in<type>
{
public:
  typedef auto_in<type> index_type;

  using typename index_type::arg_type;
  using typename index_type::base_type;
  using typename index_type::shared_type;

  using index_type::is_input;
  using index_type::is_output;

  using index_type::operator typename index_type::base_type;
  operator typename index_type::shared_type () {
    return *static_cast<auto_in<type>*>(this);
  }

  auto_arg(std::shared_ptr<AutoPacket> packet):
    auto_in<type>(packet)
  {}
};

template<class type>
class auto_arg<auto_in<type>>:
public auto_in<type>
{
public:
  typedef auto_in<type> index_type;

  using typename index_type::arg_type;
  using typename index_type::base_type;
  using typename index_type::shared_type;

  using index_type::is_input;
  using index_type::is_output;

  using index_type::operator typename index_type::base_type;
  operator typename index_type::shared_type () {
    return *static_cast<auto_in<type>*>(this);
  }

  auto_arg(std::shared_ptr<AutoPacket> packet):
  auto_in<type>(packet)
  {}
};

/*
class auto_out


class auto_pipe


class auto_actor
*/
