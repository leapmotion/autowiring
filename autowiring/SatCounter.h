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
    remaining(0),
    optional(0)
  {}

  SatCounter(const AutoFilterDescriptor& source):
    AutoFilterDescriptor(source),
    called(false),
    remaining(0),
    optional(0)
  {}

  SatCounter(const SatCounter& source):
    AutoFilterDescriptor(static_cast<const AutoFilterDescriptor&>(source)),
    called(source.called),
    remaining(source.remaining),
    optional(source.optional)
  {}

  SatCounter& operator = (const SatCounter& source) {
    AutoFilterDescriptor::operator = (source);
    called = source.called;
    remaining = source.remaining;
    optional = source.optional;
    return *this;
  }

  // The number of times the AutoFilter is called
  bool called;

  // The REQUIRED remaining counter:
  size_t remaining;

  // The OPTIONAL remaining counter:
  size_t optional;

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
  /// Resets the optional and remaining counters to their initial values
  /// </summary>
  void Reset(void) {
    called = false;
    remaining = m_requiredCount;
    optional = m_optionalCount;
  }

  /// <summary>
  /// Conditionally decrements AutoFilter argument satisfaction.
  /// </summary>
  /// <returns>True if this decrement yielded satisfaction of all arguments</returns>
  bool Decrement(const std::type_index& data, bool is_mandatory) {
    is_mandatory ? --remaining : --optional;
    return remaining == 0 && optional == 0;
  }

  /// <summary>
  /// Conditionally increments AutoFilter argument satisfaction.
  /// </summary>
  void Increment(const std::type_index& data, bool is_mandatory) {
    is_mandatory ? ++remaining : ++optional;
  }

  /// <summary>
  /// Sets the optional count to zero if mandatory inputs are satisfied
  /// </summary>
  /// <returns>True if all mandatory arguments are satisfied</returns>
  bool Resolve() {
    if (IsDeferred())
      // IMPORTANT: Deferred calls cannot be finalized
      return false;

    if (remaining == 0 && optional != 0) {
      optional = 0;
      return true;
    }
    return false;
  }

  /// <returns>False if there are any mandatory or optional elements still outstanding</returns>
  operator bool(void) const { return !remaining && !optional; }
};
