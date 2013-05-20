#include "stdafx.h"
#include "AutoNetworkMonitor.h"
#include "AutowirableSlot.h"

boost::thread_specific_ptr<AutoNetworkMonitor::Space> AutoNetworkMonitor::s_space;

AutoNetworkMonitor::AutoNetworkMonitor(void)
{
}

AutoNetworkMonitor::~AutoNetworkMonitor(void)
{
}

void AutoNetworkMonitor::PreConstruct(void* pMem, size_t szSpace) {
  Space* pSpace = s_space.get();
  if(!pSpace) {
    pSpace = new Space;
    s_space.reset(pSpace);
  }

  // Register:
  pSpace->m_mp[Allocation(pMem, szSpace)];
}

void AutoNetworkMonitor::Notify(const AutowirableSlot& lhs) {
  Space* pSpace = s_space.get();
  if(!pSpace)
    return;

  auto q = pSpace->Find(&lhs);
  if(q != pSpace->m_mp.end())
    q->second.push_back(lhs.m_tracker);
}

void AutoNetworkMonitor::PostConstruct(void* pSpace) {
  // Locate the space:

  // Resolve all allocations:
  
  // Lock and map:
}

void AutoNetworkMonitor::Serialize(std::ostream& os) const {
}

std::ostream& operator<<(std::ostream& os, const AutoNetworkMonitor& rhs) {
  return rhs.Serialize(os), os;
}