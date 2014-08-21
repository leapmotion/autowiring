// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"
#include "AutoFilterDescriptor.h"

/// <summary>
/// A single subscription counter entry
/// </summary>
struct SatCounter:
  public AutoFilterDescriptor
{
  SatCounter(const AutoFilterDescriptor& rhs) :
    AutoFilterDescriptor(rhs)
  {}

  // The MANDATORY remaining counter:
  size_t remaining;

  // The OPTIONAL remaining counter:
  size_t optional;

  /// <summary>
  /// Calls the underlying AutoFilter method with the specified AutoPacketAdapter as input
  /// </summary>
  void CallAutoFilter(AutoPacket& packet) {
    GetCall()(GetAutoFilter()->ptr(), packet);
  }

  /// <summary>
  /// Resets the optional and remaining counters to their initial values
  /// </summary>
  void Reset(void) {
    remaining = m_requiredCount;
    optional = m_optionalCount;
  }

  bool IsInput(const std::type_index& data, const std::type_index& source) {
    auto dataFlow = m_dataMap.find(data);
    if (dataFlow != m_dataMap.end()) {
      if (dataFlow->second.broadcast) {
        if (source == typeid(void)) {
          return true;
        }
      } else {
        if (dataFlow->second.halfpipes.find(source) != dataFlow->second.halfpipes.end()) {
          return true;
        }
      }
    }
    return false;
  }

  /// <summary>
  /// Conditionally decrements AutoFilter argument satisfaction.
  /// </summary>
  /// <returns>True if this decrement yielded satisfaction of all arguments</returns>
  bool Decrement(const std::type_index& data, const std::type_index& source, bool is_mandatory) {
    if (IsInput(data, source)) {
      is_mandatory ? --remaining : --optional;
      return remaining == 0 && optional == 0;
    }
    return false;
  }

  /// <summary>
  /// Conditionally increments AutoFilter argument satisfaction.
  /// </summary>
  void Increment(const std::type_index& data, const std::type_index& source, bool is_mandatory) {
    if (IsInput(data, source)) {
      is_mandatory ? ++remaining : ++optional;
    }
  }

  /// <summary>
  /// Removes all remaining
  /// </summary>
  /// <returns>True if all mandatory arguments are satisfied</returns>
  bool Resolve() {
    if (remaining == 0 &&
        optional != 0) {
        optional = 0;
        return true;
    }
    return false;
  }

  /// <returns>False if there are any mandatory or optional elements still outstanding</returns>
  operator bool(void) const { return !remaining && !optional; }
};
