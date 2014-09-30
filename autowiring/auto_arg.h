// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include "auto_in.h"
#include "auto_out.h"
#include "optional_ptr.h"

/*
 The auto_arg<T> classes are used to generate of auto_in and auto_out types
 based on the argument type.
 The core functionality is that all argument types are treated as standard
 types, and can be re-derived from those standard types.
 It is expected that every parent of auto_arg will define:
 - id_type (an unqualified type)
 - base_type (a type qualified by const and/or &)
 - shared_type (the pointer type from which the parent inherits)
 Furthermore, by classifying arguments in terms of orientation and fundamental
 type, auto_arg enables the specification of a principal type to any class
 implementing an AutoFilter method.
 */

/// <summary>
/// Reinterpret copied argument as input
/// </summary>
template<class type>
class auto_arg:
  public auto_in<type>
{
public:
  typedef auto_in<type> auto_type;

  auto_arg() {}

  auto_arg(std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)):
    auto_in<type>(packet, source)
  {}

  static const bool is_shared = false;
  static const bool is_optional = false;
};

/// <summary>
/// Reinterpret copied argument as input
/// </summary>
template<class type>
class auto_arg<const type>:
  public auto_in<type>
{
public:
  typedef auto_in<type> auto_type;

  auto_arg() {}

  auto_arg(std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)):
    auto_in<type>(packet, source)
  {}

  static const bool is_shared = false;
  static const bool is_optional = false;
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

  auto_arg() {}

  auto_arg(std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)):
    auto_in<type>(packet, source)
  {}

  static const bool is_shared = false;
  static const bool is_optional = false;
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

  auto_arg() {}

  auto_arg(std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)):
    auto_in<type>(packet, source)
  {}

  static const bool is_shared = true;
  static const bool is_optional = false;
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

  auto_arg() {}

  auto_arg(std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)):
    auto_in<type>(packet, source)
  {}

  static const bool is_shared = true;
  static const bool is_optional = false;
};

/// <summary>
/// Specialization for "T&" ~ auto_in<T>
/// </summary>
template<class type>
class auto_arg<type&>:
  public auto_out<type>
{
public:
  typedef auto_out<type> auto_type;

  auto_arg() {}

  auto_arg(std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)):
    auto_out<type>(packet, source)
  {}

  static const bool is_shared = false;
  static const bool is_optional = false;
};

/// <summary>
/// Specialization for "std::unique_ptr<T, std::function<void(T*)>>" ~ auto_out<T>
/// </summary>
template<class type>
class auto_arg<std::shared_ptr<type>>:
  public auto_out<type>
{
public:
  typedef auto_out<type> auto_type;

  auto_arg() {}

  auto_arg(std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)):
    auto_out<type>(packet, source)
  {}

  static const bool is_shared = true;
  static const bool is_optional = false;
};

/// <summary>
/// Specialization for equivalent type auto_out<T>
/// </summary>
template<class type>
class auto_arg<auto_out<type>>:
  public auto_out<type>
{
public:
  typedef auto_out<type> auto_type;

  auto_arg() {}

  auto_arg(std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)):
    auto_out<type>(packet, source)
  {}

  static const bool is_shared = true;
  static const bool is_optional = true;
};

/// <summary>
/// Specialization for equivalent type optional_ptr<T>
/// </summary>
template<class type>
class auto_arg<optional_ptr<type>>:
  public optional_ptr<type>
{
public:
  typedef optional_ptr<type> auto_type;

  auto_arg() {}

  auto_arg(std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)):
    optional_ptr<type>(packet, source)
  {}

  static const bool is_shared = true;
  static const bool is_optional = true;
};

/// <summary>
/// Specialization for first-call "AutoPacket&"
/// </summary>
template<>
class auto_arg<AutoPacket&>
{
protected:
  /// Sigil to distinguish AutoPacket&
  class first_call_sigil {};

  std::shared_ptr<AutoPacket> m_packet;

public:
  typedef AutoPacket& auto_type;

  typedef first_call_sigil id_type;
  typedef AutoPacket& base_type;
  typedef std::shared_ptr<AutoPacket> shared_type;

  // Although AutoPacket& enable both inputs and outputs
  // its availability is handled as an input.
  static const bool is_input = true;
  static const bool is_output = false;

  operator base_type () const {
    return *m_packet;
  }
  operator shared_type () {
    return m_packet;
  }

  auto_arg() {}

  auto_arg(std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)):
    m_packet(packet)
  {}

  static const bool is_shared = false;
  static const bool is_optional = false;
};

/// <summary>
/// Specialization for final-call "const AutoPacket&"
/// </summary>
/// <remarks>
/// This is called during Finalize, so the shared_ptr may
/// be invalidated if the associated ObjectPool is cleared.
/// Therefore it is essential that this cannot be used with
/// deferred calls.
/// </remarks>
template<>
class auto_arg<const AutoPacket&>
{
protected:
  /// Sigil to distinguish const AutoPacket&
  class final_call_sigil {};

  std::shared_ptr<AutoPacket> m_packet;

public:
  typedef const AutoPacket& auto_type;

  typedef final_call_sigil id_type;
  typedef const AutoPacket& base_type;
  typedef std::weak_ptr<const AutoPacket> shared_type;

  // const AutoPacket& can only be used to observe data
  static const bool is_input = true;
  static const bool is_output = false;

  operator base_type () const {
    return *m_packet;
  }
  operator shared_type () {
    return m_packet;
  }

  auto_arg() {}
  
  auto_arg(std::shared_ptr<AutoPacket> packet, const std::type_info& source = typeid(void)):
    m_packet(packet)
  {}

  static const bool is_shared = false;
  static const bool is_optional = false;
};
