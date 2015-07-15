// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once

#include "auto_out.h"
#include CHRONO_HEADER

/// <summary>
/// Adds a time-of-creation to every AutoPacket created in a context
/// that includes AutoTimeStamp.
/// </summary>
class AutoTimeStamp {
public:
  /// <summary>Records time of issue of AutoPacket</summary>
  /// <remarks>
  /// Default constructor yielding current time is called by auto_out<time>.
  /// AutoPacketFactory.NewPacket()->Has<AutoTimeStamp::time>() == true.
  /// </remarks>
  void AutoFilter(auto_out<std::chrono::high_resolution_clock::time_point> t) {
    *t = std::chrono::high_resolution_clock::now();
  }
};
