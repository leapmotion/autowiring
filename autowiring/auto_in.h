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
  typedef T id_type;
  static const bool is_input = true;
  static const bool is_output = false;

  static const T& Get(AutoPacket& packet) {
    const T* out;
    packet.Get(out);
    return *out;
  }

  auto_in(AutoPacket& packet) :
    m_value(Get(packet))
  {}

private:
  const T& m_value;

public:
  operator const T&() const { return m_value; }
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