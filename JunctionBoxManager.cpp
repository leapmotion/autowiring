// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "TypeRegistry.h"
#include "JunctionBoxManager.h"
#include "AutoPacketFactory.h"
#include "AutoPacketListener.h"

JunctionBoxManager::JunctionBoxManager(void) {
  // Enumerate all Autofired types to initialize a new JunctionBox for each
  for(auto p = g_pFirstEntry; p; p = p->pFlink)
    m_junctionBoxes[p->ti] = p->m_NewJunctionBox();
  
  // Manually add internal types
  m_junctionBoxes[typeid(AutoPacketListener)] = std::make_shared<JunctionBox<AutoPacketListener>>();
}

JunctionBoxManager::~JunctionBoxManager(void) {}

std::shared_ptr<JunctionBoxBase> JunctionBoxManager::Get(std::type_index pTypeIndex) {
  auto box = m_junctionBoxes.find(pTypeIndex);
  assert(box != m_junctionBoxes.end());
  
  //Check here if any listening marshals might be interested in receiving the fired args
  auto mapfinditerator = m_eventOutputStreams.find(pTypeIndex);
  std::vector<std::weak_ptr<EventOutputStreamBase> > * OutputStreamVector = nullptr;
  if (mapfinditerator != m_eventOutputStreams.end()){
    //no vec on this type yet. So create it, pass it, and wait for it to get filled later
    OutputStreamVector = &(mapfinditerator->second);
  }
  else {
    std::vector<std::weak_ptr<EventOutputStreamBase> > newvec;
    m_eventOutputStreams[pTypeIndex] = newvec; //assignment copy constructor invoked; 
    auto it  = m_eventOutputStreams.find(pTypeIndex);
    OutputStreamVector = &(it->second);
  }

  (box->second)->SetPotentialMarshals(OutputStreamVector);
  return box->second;
}

void JunctionBoxManager::AddEventReceiver(std::shared_ptr<EventReceiver> pRecvr){
  
  //Notify all junctionboxes that there is a new event
  for(auto q = m_junctionBoxes.begin(); q != m_junctionBoxes.end(); q++)
    *(q->second) += pRecvr;
}

void JunctionBoxManager::RemoveEventReceiver(std::shared_ptr<EventReceiver> pRecvr){
  
  // Notify all compatible senders that we're going away:
  for(auto q = m_junctionBoxes.begin(); q != m_junctionBoxes.end(); q++)
    *(q->second) -= pRecvr;
}

void JunctionBoxManager::RemoveEventReceivers(t_rcvrSet::const_iterator first, t_rcvrSet::const_iterator last){
  
  for(auto r = m_junctionBoxes.begin(); r != m_junctionBoxes.end(); r++) {
    auto box = r->second;
    for(auto q = first; q != last; q++) {
      *box -= *q;
    }
  }
}
