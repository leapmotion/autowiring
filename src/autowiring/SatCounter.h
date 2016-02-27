// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoFilterDescriptor.h"

/// <summary>
/// A single subscription counter entry
/// </summary>
struct SatCounter:
  AutoFilterDescriptor
{
  SatCounter(void) = default;

  SatCounter(const AutoFilterDescriptor& source):
    AutoFilterDescriptor(source),
    remaining(m_requiredCount)
  {}

  SatCounter(const SatCounter& source):
    AutoFilterDescriptor(static_cast<const AutoFilterDescriptor&>(source)),
    remaining(source.remaining)
  {}

  // Forward and backward linked list pointers
  SatCounter* flink = nullptr;
  SatCounter* blink = nullptr;

  // The number of inputs remaining to this counter:
  size_t remaining = 0;

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

namespace std {
  template<>
  struct hash<SatCounter>
  {
    size_t operator()(const SatCounter& satCounter) const {
      return (size_t)satCounter.GetAutoFilter().ptr();
    }
  };
}
