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
    flink(nullptr),
    blink(nullptr),
    remaining(0),
    called(false)
  {}

  SatCounter(const AutoFilterDescriptor& source):
    AutoFilterDescriptor(source),
    flink(nullptr),
    blink(nullptr),
    remaining(m_requiredCount),
    called(false)
  {}

  SatCounter(const SatCounter& source):
    AutoFilterDescriptor(static_cast<const AutoFilterDescriptor&>(source)),
    flink(nullptr),
    blink(nullptr),
    remaining(source.remaining),
    called(source.called)
  {}

  // Forward and backward linked list pointers
  SatCounter* flink;
  SatCounter* blink;

  // The number of inputs remaining to this counter:
  size_t remaining;

  // Has the associated function been called?
  bool called;

private:
  /// <summary>
  /// Throws a formatted exception if the underlying filter is called more than once
  /// </summary>
  void ThrowRepeatedCallException(void) const;

public:
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
};
