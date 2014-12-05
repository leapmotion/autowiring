// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_in.h"
#include "auto_out.h"

/*
 The auto_arg<T> classes are used to generate of auto_in and auto_out types
 based on the argument T.
 The core functionality is that all argument types are treated as standard
 types, and can be re-derived from those standard types.
 It is expected that every parent of auto_arg will define:
 - id_type (an unqualified T)
 - base_type (a T qualified by const and/or &)
 - shared_type (the pointer T from which the parent inherits)
 Furthermore, by classifying arguments in terms of orientation and fundamental
 T, auto_arg enables the specification of a principal T to any class
 implementing an AutoFilter method.
 */

/// <summary>
/// Reinterpret copied argument as input
/// </summary>
template<class T>
class auto_arg:
  public auto_in<T>
{
public:
  auto_arg(AutoPacket& packet):
    auto_in<T>(packet)
  {}

  static const bool is_shared = false;
};

/// <summary>
/// Reinterpret copied argument as input
/// </summary>
template<class T>
class auto_arg<const T>:
  public auto_in<T>
{
public:
  auto_arg(AutoPacket& packet):
    auto_in<T>(packet)
  {}

  static const bool is_shared = false;
};

/// <summary>
/// Specialization for "const T&" ~ auto_in<T>
/// </summary>
template<class T>
class auto_arg<const T&>:
  public auto_in<T>
{
public:
  auto_arg(AutoPacket& packet):
    auto_in<T>(packet)
  {}

  static const bool is_shared = false;
};

/// <summary>
/// Specialization for "std::shared_ptr<const T>" ~ auto_in<T>
/// </summary>
template<class T>
class auto_arg<std::shared_ptr<const T>>:
  public auto_in<std::shared_ptr<const T>>
{
public:
  typedef T id_type;

  auto_arg(AutoPacket& packet):
    auto_in<std::shared_ptr<const T>>(packet)
  {}

  static const bool is_shared = true;
};

/// <summary>
/// Specialization for equivalent T auto_in<T>
/// </summary>
template<class T>
class auto_arg<auto_in<T>>:
  public auto_in<T>
{
public:
  auto_arg(AutoPacket& packet):
    auto_in<T>(packet)
  {}

  static const bool is_shared = true;
};

/// <summary>
/// Specialization for "T&" ~ auto_in<T>
/// </summary>
template<class T>
class auto_arg<T&> :
  public auto_out<T>
{
public:
  typedef auto_out<T> auto_type;

  auto_arg(AutoPacket& packet) :
    auto_out<T>(packet)
  {}

  static const bool is_shared = false;
};

/// <summary>
/// Specialization for "std::shared_ptr<T>&" ~ auto_in<T>
/// </summary>
template<class T>
class auto_arg<std::shared_ptr<T>&> :
  public auto_out<T>
{
public:
  auto_arg(AutoPacket& packet) :
    auto_out<T>(packet)
  {}

  static const bool is_shared = false;
};

/// <summary>
/// Forbidden input T
/// </summary>
template<class T>
class auto_arg<std::shared_ptr<T>> {
  static_assert(
    std::is_same<T, AutoPacket>::value,
    "std::shared_ptr<AutoPacket> is the only T that may be supplied as a non-const shared input"
  );
};

/// <summary>
/// Specialization for equivalent T auto_out<T>
/// </summary>
template<class T>
class auto_arg<auto_out<T>>:
  public auto_out<T>
{
public:
  auto_arg(AutoPacket& packet):
    auto_out<T>(packet)
  {}

  static const bool is_shared = true;
};

/// <summary>
/// Specialization for first-call "AutoPacket&"
/// </summary>
template<>
class auto_arg<AutoPacket&>
{
public:
  auto_arg(AutoPacket& packet) :
    m_packet(packet)
  {}

protected:
  /// Sigil to distinguish AutoPacket&
  class first_call_sigil {
  public:
    first_call_sigil(void);
    virtual ~first_call_sigil(void);
  };

  AutoPacket& m_packet;

public:
  typedef first_call_sigil id_type;

  // Although AutoPacket& enable both inputs and outputs
  // its availability is handled as an input.
  static const bool is_input = true;
  static const bool is_output = false;

  operator AutoPacket&(void) const {
    return m_packet;
  }
  operator std::shared_ptr<AutoPacket>(void) {
    return m_packet.shared_from_this();
  }

  static const bool is_shared = false;
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
class auto_arg<const AutoPacket&>:
  public auto_arg<AutoPacket&>
{
public:
  auto_arg(const AutoPacket& packet) :
    auto_arg<AutoPacket&>(const_cast<AutoPacket&>(packet))
  {}
};
