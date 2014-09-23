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
    AutoFilterDescriptor(source),
    called(source.called),
    remaining(source.remaining),
    optional(source.optional),
    satisfaction(source.satisfaction)
  {}

  SatCounter& operator = (const SatCounter& source) {
    AutoFilterDescriptor::operator = (source);
    called = source.called;
    remaining = source.remaining;
    optional = source.optional;
    satisfaction = source.satisfaction;
    return *this;
  }

  // The number of times the AutoFilter is called
  bool called;

  // The REQUIRED remaining counter:
  size_t remaining;

  // The OPTIONAL remaining counter:
  size_t optional;

  // The sources satisfying each argument
  autowiring::DataFill satisfaction;

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
    GetCall()(GetAutoFilter()->ptr(), packet, satisfaction);
  }

  /// <summary>
  /// Resets the optional and remaining counters to their initial values
  /// </summary>
  void Reset(void) {
    called = false;
    remaining = m_requiredCount;
    optional = m_optionalCount;
    satisfaction.clear();
    satisfaction.reserve(m_requiredCount + m_optionalCount);

    // Insert this type as a provider of output arguments
    for (auto& data : m_dataMap) {
      if (data.second.output) {
        satisfaction[data.first] = m_pType;
      }
    }
  }

  bool IsInput(const std::type_index& data, const std::type_info& source) const {
    auto dataFlow = m_dataMap.find(data);
    if (dataFlow != m_dataMap.end()) {
      if (source == typeid(void)) {
        if (dataFlow->second.broadcast) {
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
  bool Decrement(const std::type_index& data, const std::type_info& source, bool is_mandatory) {
    if (IsInput(data, source)) {
      if (satisfaction.find(data) != satisfaction.end()) {
        std::stringstream ss;
        ss << "Repeated data type " << autowiring::demangle(data)
        << " for " << m_pType->name()
        << " provided by " << autowiring::demangle(satisfaction.find(data)->second)
        << " and also by " << autowiring::demangle(source)
        << std::endl;
        throw std::runtime_error(ss.str());
      }
      satisfaction[data] = &source;
      is_mandatory ? --remaining : --optional;
      return remaining == 0 && optional == 0;
    }
    return false;
  }

  /// <summary>
  /// Conditionally increments AutoFilter argument satisfaction.
  /// </summary>
  void Increment(const std::type_index& data, const std::type_info& source, bool is_mandatory) {
    if (IsInput(data, source)) {
      is_mandatory ? ++remaining : ++optional;
    }
  }

  /// <summary>
  /// Removes all remaining
  /// </summary>
  /// <returns>True if all mandatory arguments are satisfied</returns>
  bool Resolve() {
    if (IsDeferred())
      // IMPORTANT: Deferred calls cannot be finalized
      return false;
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
