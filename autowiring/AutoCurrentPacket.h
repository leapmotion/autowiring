// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoPacket.h"

/// <summary>
/// RAII for AutoPacket::SetCurrent
/// </summary>
class AutoCurrentPacket
{
public:
  AutoCurrentPacket(AutoPacket& apkt) {
    AutoPacket::SetCurrent(apkt.shared_from_this());
  };
  ~AutoCurrentPacket(void) {
    AutoPacket::SetCurrent(nullptr);
  };
};
