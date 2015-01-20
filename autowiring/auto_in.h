// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include "AutoPacket.h"
#include MEMORY_HEADER

/// <summary>
/// Fundamental type of required input arguments of AutoFilter methods.
/// </summary>
template<class T>
class auto_in
{
public:
  typedef std::shared_ptr<T> id_type;
  static const bool is_input = true;
  static const bool is_output = false;

  auto_in(AutoPacket& packet) :
    packet(packet),
    m_value(packet.Get<T>())
  {}

private:
  const AutoPacket& packet;
  const T& m_value;

public:
  DEPRECATED(operator bool(void) const, "This test is unnecessary, auto_in will always be satisfied") {
    return true;
  }

  const std::shared_ptr<const T>& get(void) const {
    const std::shared_ptr<const T>* retVal;
    if(!packet.Get(retVal))
      throw std::runtime_error("Shared pointer not available on this type");
    return *retVal;
  }

  operator const T&() const { return m_value; }
  operator std::shared_ptr<const T>() const { return get(); }
  const T& operator*(void) const { return m_value; }
  const T* operator->(void) const { return &m_value; }
};


template<>
class auto_in<AutoPacket>
{
public:
  auto_in(AutoPacket& packet) :
    packet(packet)
  {}

private:
  AutoPacket& packet;

public:
  operator AutoPacket&() const { return packet; }
  AutoPacket& operator*(void) const { return packet; }
  AutoPacket* operator->(void) const { return &packet; }
};

template<class T>
class auto_in<T const *[]>
{
public:
  typedef auto_id<T> id_type;
  typedef auto_in<T const *[]> type;
  static const int arg_type = AutoArgType::In | AutoArgType::Multi;
  static const int tshift = 0;

  auto_in(AutoPacket& packet) :
    packet(packet),
    m_values(packet.GetAll<T>())
  {}

private:
  const AutoPacket& packet;
  const std::vector<const T*> m_values;

public:
  operator const T* const *() const { return &m_values[0]; }
};
