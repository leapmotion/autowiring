// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "CallExtractor.h"
#include "AutoPacket.h"

using namespace autowiring;

CESetup<>::CESetup(AutoPacket& packet) :
  pshr(packet.GetContext())
{}
