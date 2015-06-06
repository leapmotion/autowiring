// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include MEMORY_HEADER
#include <vector>

class AutoPacket;

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

  auto_in(const T& value) :
    m_value(value)
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

template<class T>
class auto_in<T const **>
{
public:
  auto_in(std::vector<const T*>&& values) :
    m_values(std::move(values))
  {}

private:
  const AutoPacket& packet;
  std::vector<const T*> m_values;

public:
  operator const T**() { return &m_values[0]; }
};
