// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <autowiring/auto_out.h>
#include CHRONO_HEADER

/// <summary>
/// Adds a time-of-creation to every AutoPacket created in a context
/// that includes AutoTimeStamp.
/// </summary>
class AutoTimeStamp {
public:
  /// <summary>Alias type for maximum precision timing<summary>
  class time : public std::chrono::high_resolution_clock::time_point {};

  /// <summary>Records time of issue of AutoPacket</summary>
  /// <remarks>
  /// Default constructor yielding current time is called by auto_out<time>.
  /// AutoPacketFactory.NewPacket()->Has<AutoTimeStamp::time>() == true.
  /// </remarks>
  void AutoFilter(auto_out<time> t) {
    t.make(); // Invoke construction here
  }
};
