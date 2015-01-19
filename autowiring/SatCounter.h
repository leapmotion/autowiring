// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"
#include "AutoFilterDescriptor.h"
#include "demangle.h"

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
    remaining(m_requiredCount)
  {}

  SatCounter(const SatCounter& source):
    AutoFilterDescriptor(static_cast<const AutoFilterDescriptor&>(source)),
    called(source.called),
    remaining(source.remaining)
  {}

  // The number of times the AutoFilter is called
  bool called;

  // The number of inputs remaining to this counter:
  size_t remaining;

private:
  /// <summary>
  /// Throws a formatted exception if the underlying filter is called more than once
  /// </summary>
  void ThrowRepeatedCallException(void) const;

public:
  /// <summary>
  /// Calls the underlying AutoFilter method with the specified AutoPacketAdapter as input
  /// </summary>
  void CallAutoFilter(AutoPacket& packet) {
    if (called)
      ThrowRepeatedCallException();
    called = true;
    GetCall()(GetAutoFilter(), packet);
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
