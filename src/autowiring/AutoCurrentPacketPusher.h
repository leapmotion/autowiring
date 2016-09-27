// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoPacket.h"

namespace autowiring {

/// <summary>
/// RAII for AutoPacket::SetCurrent
/// </summary>
class AutoCurrentPacketPusher
{
public:
  AutoCurrentPacketPusher(AutoPacket& apkt):
    prior(AutoPacket::SetCurrent(&apkt))
  {}

  ~AutoCurrentPacketPusher(void) {
    AutoPacket::SetCurrent(prior);
  };

private:
  AutoPacket* const prior;
};

}
