// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ObjectPoolMonitor.h"

ObjectPoolMonitor::ObjectPoolMonitor(void* pOwner) :
  m_pOwner(pOwner)
{}

void ObjectPoolMonitor::Abandon(void) {
  (std::lock_guard<std::mutex>)*this,
  m_abandoned = true;
}
