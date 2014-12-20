// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"
#include "AutoFilterDescriptor.h"
#include "demangle.h"
#include <sstream>

/// <summary>
/// A single subscription counter entry
/// </summary>
struct SatCounter:
  public AutoFilterDescriptor
{
  SatCounter(void):
    called(false),
    remaining(0)
  {}

  SatCounter(const AutoFilterDescriptor& source):
    AutoFilterDescriptor(source),
    called(false),
    remaining(0)
  {}

  SatCounter(const SatCounter& source):
    AutoFilterDescriptor(static_cast<const AutoFilterDescriptor&>(source)),
    called(source.called),
    remaining(source.remaining)
  {}

  SatCounter& operator = (const SatCounter& source) {
    AutoFilterDescriptor::operator = (source);
    called = source.called;
    remaining = source.remaining;
    return *this;
  }

  // The number of times the AutoFilter is called
  bool called;

  // The number of inputs remaining to this counter:
  size_t remaining;

  /// <summary>
  /// Calls the underlying AutoFilter method with the specified AutoPacketAdapter as input
  /// </summary>
  void CallAutoFilter(AutoPacket& packet) {
    if (called) {
      std::stringstream ss;
      ss << "Repeated call to " << autowiring::demangle(m_pType);
      throw std::runtime_error(ss.str());
    }
    called = true;
    GetCall()(GetAutoFilter(), packet);
  }

  /// <summary>
  /// Resets the remaining counter to its initial value
  /// </summary>
  void Reset(void) {
    called = false;
    remaining = m_requiredCount;
  }

  /// <summary>
  /// Conditionally decrements AutoFilter argument satisfaction.
  /// </summary>
  /// <returns>True if this decrement yielded satisfaction of all arguments</returns>
  bool Decrement(void) {
    return !--remaining;
  }

  /// <summary>
  /// Conditionally increments AutoFilter argument satisfaction.
  /// </summary>
  void Increment(void) {
    ++remaining;
  }

  /// <returns>False if there are any inputs still outstanding</returns>
  operator bool(void) const { return !remaining; }
};
