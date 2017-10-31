// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ObjectPoolMonitor.h"

using namespace autowiring;

ObjectPoolMonitor::ObjectPoolMonitor(void) {}

void ObjectPoolMonitor::Abandon(void) {
  (std::lock_guard<std::mutex>)*this,
  m_abandoned = true;
}
