// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoPacket.h"

/// <summary>
/// RAII for AutoPacket::SetCurrent
/// </summary>
namespace autowiring {
    class AutoCurrentPacketPusher
    {
    public:
        AutoCurrentPacketPusher(AutoPacket& apkt) {
            AutoPacket::SetCurrent(apkt.shared_from_this());
        };
        ~AutoCurrentPacketPusher(void) {
            AutoPacket::SetCurrent(nullptr);
        };
    };
}
