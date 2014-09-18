// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

/// <summary>Class to automatically generate auto_in and auto_out</summary>
/// <remarks>
/// The static definitions in this class can be used to enable
/// argument type validation
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


template <class type>
class auto_in:
  public std::shared_ptr<const type>
{
public:
  typedef type arg_type;
  typedef const type& base_type;
  typedef std::shared_ptr<const type> shared_type;

  static const bool is_input = true;
  static const bool is_output = false;

  operator base_type () const {
    return *(this->get());
  }

  auto_in (const AutoPacket& packet) {
    packet.Get(*this);
  }
};

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
  operator typename index_type::shared_type () {
    return *static_cast<auto_in<type>*>(this);
  }

  auto_arg(const AutoPacket& packet):
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

  auto_arg(const AutoPacket& packet):
    auto_in<type>(packet)
  {}
};

/*
class auto_out


class auto_pipe


class auto_actor
*/
