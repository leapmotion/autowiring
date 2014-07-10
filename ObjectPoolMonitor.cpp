#include "stdafx.h"
#include "ObjectPoolMonitor.h"
#include <boost/thread/locks.hpp>

ObjectPoolMonitor::ObjectPoolMonitor(void* pOwner) :
  m_pOwner(pOwner),
  m_abandoned(false)
{
}

void ObjectPoolMonitor::Abandon(void) {
  (std::lock_guard<std::mutex>)*this,
  m_abandoned = true;
}