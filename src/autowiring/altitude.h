// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once

namespace autowiring {

/// <summary>
/// Defines the altitude enumeration concept for AutoFilter instances
/// </summary>
/// <remarks>
/// A filter altitude is an indicator to the AutoFilter scheduler about when a particular filter
/// should be scheduled to receive control.  Altitude is a hard requirement, but is subject to
/// a number of stipulations as to when it applies:
///
///  1)  If two autofilters are both candidates to be run at the same time, the filter with the
///      higher altitude will be run first.
///  2)  If both filters have the same altitude, an arbitrary filter will be selected.
///  3)  When the current filter returns control (IE, its AutoFilter routine returns), the next
///      filter will be run.
///  4)  Deferred AutoFilters are a special case.  A deferred AutoFilter is considered to have
///      returned control as soon as its execution has been scheduled; generally this happens very
///      fast.
///  5)  Altitudes only provide an order-of-execution guarantee if NO deferred AutoFilters have been
///      declared in the network.
/// </remarks>
enum class altitude {
  // Highest altitude level.  Reserved for temporary debug logic and other nonpermanent code that
  // must run before all other filter levels
  Highest =                 0x10000,

  // Instrumentation level, for use with instrumentation code.  Instrumentation code often needs to
  // observe the inputs to its AutoFilter before any other code has an opportunity to observe it,
  // because this code needs information about exactly when a decoration was first available.
  Instrumentation =         0x20000,

  // Default altitude for Deferred autofilters.  Deferred autofilters are guaranteed to return very
  // quickly, even though they may do a lot of work, because they do not tie up the main thread.
  Dispatch =                0x30000,

  // Asynchronous filters are designed to be run with a higher priority than standard filters,
  // but are still expected to complete very quickly.  Because their speedy behavior is implemented
  // by the filter, and not guaranteed by Autowiring, asynchronous filters are considered to
  // have a lower priority than Deferred filters.
  //
  // It is expected that AutoFilters which are marked as Asynchronous will do the majority of their
  // work in an std::async or other similar call.
  Asynchronous =            0x40000,

  // The realtime altitude is a higher-than-normal altitude which may have some tight timing requirements
  // but does not run in a separate thread.  Realtime filters run after deferred filters have been
  // scheduled to run.
  Realtime =                0x50000,

  // Default altitude range.  Unless otherwise specified, or the filter is marked Deferred, filters
  // will normally execute at this priority level.
  Standard =                0x60000,

  // Altitude indicator for filters with no hard timing requirements.  This is a convenient place to put
  // filters that may have extensive CPU usage requirements, or which are not strongly impacted by timing.
  // Analytics and diagnostics are typically suitable for execution at the passive level.
  Passive =                 0x70000,

  // Lowest altitude level.  Reserved for temporary debug logic and other nonpermanent code that
  // must run after all other filter levels.
  Lowest =                  0x10000000
};

inline altitude operator+(altitude alt, int v) {
  return (altitude) ((int) alt + v);
}

inline altitude operator-(altitude alt, int v) {
  return (altitude)((int)alt - v);
}

/// <summary>
/// Extracts the altitude of type T, if declared, or infers it if not
/// </summary>
/// <param name="T">The outer type of the AutoFilter</param>
/// <param name="Default">The default to be used if one is not provied by T</param>
template<class T, altitude Default = altitude::Standard>
struct altitude_of {
  template<class U>
  static std::integral_constant<altitude, U::altitude> select(U*);

  template<class U>
  static std::integral_constant<altitude, Default> select(...);

  static const altitude value = decltype(select<T>(nullptr))::value;
};

}
