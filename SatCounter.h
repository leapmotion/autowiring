#pragma once
#include "AnySharedPointer.h"
#include "AutoPacketSubscriber.h"

/// <summary>
/// A single subscription counter entry
/// </summary>
struct SatCounter:
  public AutoPacketSubscriber
{
  SatCounter(const AutoPacketSubscriber& rhs) :
    AutoPacketSubscriber(rhs)
  {}

  // The MANDATORY remaining counter:
  size_t remaining;

  // The OPTIONAL remaining counter:
  size_t optional;

  /// <summary>
  /// Calls the underlying AutoFilter method with the specified AutoPacketAdapter as input
  /// </summary>
  void CallAutoFilter(const AutoPacketAdaptor& adaptor) {
    GetCall()(GetSubscriberPtr(), adaptor);
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
    if(is_optional)
      optional++;
    else
      remaining++;
  }

  /// <summary>
  /// Decrements the optional, or mandatory counter based on the selection
  /// </summary>
  /// <returns>True if there are decorations yet unsatisfied, optional or not</returns>
  bool Decrement(bool is_mandatory) {
    is_mandatory ? remaining-- : optional--;
    return remaining || optional;
  }

  operator bool(void) const { return !remaining && !optional; }
};