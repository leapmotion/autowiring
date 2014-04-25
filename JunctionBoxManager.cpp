// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "JunctionBoxManager.h"
#include "AutoPacketFactory.h"
#include "AutoPacketListener.h"
#include "AutowiringEvents.h"

JunctionBoxManager::JunctionBoxManager(void) {
  // Enumerate all event types to initialize a new JunctionBox for each
  for(auto p = g_pFirstEntry; p; p = p->pFlink)
    p->m_NewJunctionBox(m_junctionBoxes[p->ti]);

  //Always allow internal events
  m_junctionBoxes[typeid(AutowiringEvents)]->Initiate();
  m_junctionBoxes[typeid(AutoPacketListener)]->Initiate();
}

JunctionBoxManager::~JunctionBoxManager(void) {}

void JunctionBoxManager::Initiate(void) {
  for (auto q = m_junctionBoxes.begin(); q != m_junctionBoxes.end(); q++)
    q->second->Initiate();
}

void JunctionBoxManager::AddEventReceiver(JunctionBoxEntry<EventReceiver> receiver) {
  // Notify all junctionboxes that there is a new event
  for(auto q = m_junctionBoxes.begin(); q != m_junctionBoxes.end(); q++)
    q->second->Add(receiver);
}

void JunctionBoxManager::AddEventReceivers(t_rcvrSet::const_iterator first, t_rcvrSet::const_iterator last) {
  // Notify all junctionboxes that there is a new event
  for(auto q = m_junctionBoxes.begin(); q != m_junctionBoxes.end(); q++)
    for(auto receiver = first; receiver != last; receiver++)
      q->second->Add(*receiver);
}

void JunctionBoxManager::RemoveEventReceiver(JunctionBoxEntry<EventReceiver> receiver) {
  // Notify all compatible senders that we're going away:
  for(auto q = m_junctionBoxes.begin(); q != m_junctionBoxes.end(); q++)
    q->second->Remove(receiver);
}

void JunctionBoxManager::RemoveEventReceivers(t_rcvrSet::const_iterator first, t_rcvrSet::const_iterator last){
  for(auto r = m_junctionBoxes.begin(); r != m_junctionBoxes.end(); r++)
    for(auto q = first; q != last; q++)
      r->second->Remove(*q);
}
