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
    GetCall()(GetAutoFilterPtr(), packet);
  }

  /// <summary>
  /// Resets the optional and remaining counters to their initial values
  /// </summary>
  void Reset(void) {
    remaining = m_requiredCount;
    optional = m_optionalCount;
  }

  /// <summary>
  /// Convenience parity method, increments the specified counter
  /// </summary>
  void Increment(bool is_optional) {
    is_optional ? ++optional : ++remaining;
  }

  /// <summary>
  /// Decrements the optional, or mandatory counter based on the selection
  /// </summary>
  /// <returns>True if this decrement yielded satisfaction of all arguments</returns>
  bool Decrement(bool is_mandatory) {
    is_mandatory ? --remaining : --optional;
    return remaining == 0 && optional == 0;
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