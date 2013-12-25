#include "stdafx.h"
#include "ObjectPoolMonitor.h"

ObjectPoolMonitor::ObjectPoolMonitor(void) :
  m_abandoned(false)
{
}

void ObjectPoolMonitor::Abandon(void) {
  (boost::lock_guard<boost::mutex>)*this,
  m_abandoned = true;
}