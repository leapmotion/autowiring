// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <autowiring/AutoPacket.h>
#include CHRONO_HEADER

/// <summary>
/// Adds a time-of-creation to every AutoPacket created in a context
/// that includes AutoTimeStamp.
/// </summary>
class AutoTimeStamp {
public:
  /// <summary>Alias type for maximum precision timing<summary>
  class time : public std::chrono::high_resolution_clock::time_point {};

  /// <summary>Called when packet is issued, adds AutoTimeStamp::time instance</summary>
  /// <remarks>
  /// AutoPacketFactory.NewPacket()->Has<AutoTimeStamp::time>() == true
  /// in a context that includes an AutoTimeStamp instance.
  /// </remarks>
  void AutoFilter(AutoPacket& packet) {
    packet.Decorate(time());
  }
};
