// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "TypeRegistry.h"
#include "JunctionBoxManager.h"
#include "AutoPacketFactory.h"
#include "AutoPacketListener.h"

JunctionBoxManager::JunctionBoxManager(void) {
  // Enumerate all Autowired types to initialize new JunctionBox for each
  for(auto p = g_pFirstEntry; p; p = p->pFlink) {
    m_junctionBoxes[p->ti] = p->m_NewJunctionBox();
  }
  
  // Manually add AutoPacketListener for CoreContext initialization
  m_junctionBoxes[typeid(AutoPacketListener)] = std::make_shared<JunctionBox<AutoPacketListener>>();
}

JunctionBoxManager::~JunctionBoxManager(void) {}

/// <summary>
/// Get the JunctionBox corresponding to type "pTypeIndex"
/// </summary>
std::shared_ptr<JunctionBoxBase> JunctionBoxManager::Get(std::type_index pTypeIndex) {
  auto box = m_junctionBoxes.find(pTypeIndex);
  assert(box != m_junctionBoxes.end());
  return box->second;
}

void JunctionBoxManager::AddEventReceiver(std::shared_ptr<EventReceiver> pRecvr){
  
  //Notify all currently used junctionboxes that there is a new event
  for(auto q = m_junctionBoxes.begin(); q != m_junctionBoxes.end(); q++){
    auto box = q->second;
    *box += pRecvr;
  }
}

void JunctionBoxManager::RemoveEventReceiver(std::shared_ptr<EventReceiver> pRecvr){
  
  // Notify all compatible senders that we're going away:
  for(auto q = m_junctionBoxes.begin(); q != m_junctionBoxes.end(); q++)
    *(q->second) -= pRecvr;
}

void JunctionBoxManager::RemoveEventReceivers(t_rcvrSet::iterator first, t_rcvrSet::iterator last){
  
  for(auto r = m_junctionBoxes.begin(); r != m_junctionBoxes.end(); r++) {
    auto box = r->second;
    for(auto q = first; q != last; q++) {
      *box -= *q;
    }
  }
}
