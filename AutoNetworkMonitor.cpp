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

AutoNetworkMonitor::Allocation::Allocation(const void* pSpace, size_t nBytes):
  pSpace(pSpace),
  nBytes(nBytes)
{
}

AutoNetworkMonitor::Space::t_mpType::iterator AutoNetworkMonitor::Space::Find(const void* ptr) {
  // Trivial return check:
  if(m_mp.empty())
    return m_mp.end();

  // Now we run a upper bound on the passed element, in order to find the first key strictly
  // greater than the pointer.  The key right before this one will be the candidate allocation
  // enclosing this pointer.
  Allocation alloc(ptr, 1);
  auto q = m_mp.upper_bound(alloc);
  if(q == m_mp.begin())
    // The first key in the map already occurs after our pointer, return here.
    return m_mp.end();
      
  q--;
  return
    q->first.Enclosed(ptr) ?

    // This pointer matches, we can return it
    q :
        
    // Found something, but it's not an enclosure of the passed pointer
    m_mp.end();
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