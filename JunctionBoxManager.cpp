// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "JunctionBoxManager.h"
#include "AutoPacketFactory.h"
#include "AutoPacketListener.h"
#include "AutowiringEvents.h"
#include "JunctionBox.h"

JunctionBoxManager::JunctionBoxManager(void) {
  // Enumerate all event types to initialize a new JunctionBox for each
  for(auto p = g_pFirstEntry; p; p = p->pFlink)
    if(p->IsEventReceiver())
      m_junctionBoxes[p->ti] = p->NewJunctionBox();

  // Ensure that these two types are specially mentioned:
  (void) RegType<AutowiringEvents>::r;

  // Always allow internal events
  m_junctionBoxes[typeid(AutowiringEvents)]->Initiate();
}

JunctionBoxManager::~JunctionBoxManager(void) {}

void JunctionBoxManager::Initiate(void) {
  for (auto q : m_junctionBoxes)
    q.second->Initiate();
}

void JunctionBoxManager::AddEventReceiver(JunctionBoxEntry<EventReceiver> receiver) {
  // Notify all junctionboxes that there is a new event
  for(auto q : m_junctionBoxes)
    q.second->Add(receiver);
}

void JunctionBoxManager::RemoveEventReceiver(JunctionBoxEntry<EventReceiver> receiver) {
  // Notify all compatible senders that we're going away:
  for(auto q : m_junctionBoxes)
    q.second->Remove(receiver);
}
