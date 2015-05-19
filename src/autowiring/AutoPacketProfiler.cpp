// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoPacketProfiler.h"

AutoPacketProfiler::AutoPacketProfiler(void) {}
AutoPacketProfiler::~AutoPacketProfiler(void) {}

void AutoPacketProfiler::AddProfilingInformation(const std::type_info& subscriber, std::chrono::nanoseconds duration) {
  m_totalTime[subscriber] += duration;
}
