#pragma once
#include <boost/any.hpp>

/// <summary>
/// A single subscription counter entry
/// </summary>
struct SatCounter {
  // The MANDATORY remaining counter:
  size_t remaining;

  // The OPTIONAL remaining counter:
  size_t optional;

  boost::any subscriber;

  /// <summary>
  /// Decrements the optional, or mandatory counter based on the selection
  /// </summary>
  /// <returns>True if there are decorations yet unsatisfied, optional or not</returns>
  bool Decrement(bool is_optional) {
    is_optional ? optional-- : remaining--;
    return remaining || optional;
  }

  /// <returns>
  /// The disposition after a decrementation, without actually performing a decrementation
  /// </returns>
  bool PseudoDecrement(bool is_optional) {
    return
      is_optional ?
      optional == 1 && !remaining :
      !optional && remaining == 1;
  }

  operator bool(void) const { return !remaining && !optional; }
};