// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "JunctionBoxManager.h"
#include "AutoPacketFactory.h"
#include "AutowiringEvents.h"
#include "JunctionBox.h"

JunctionBoxManager::JunctionBoxManager(void) {
  // Enumerate all event types to initialize a new JunctionBox for each
  for(auto p = g_pFirstEntry; p; p = p->pFlink)
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

void JunctionBoxManager::AddEventReceiver(JunctionBoxEntry<Object> receiver) {
  // Notify all junctionboxes that there is a new event
  for(auto q : m_junctionBoxes)
    q.second->Add(receiver);
}

void JunctionBoxManager::RemoveEventReceiver(JunctionBoxEntry<Object> receiver) {
  // Notify all compatible senders that we're going away:
  for(auto q : m_junctionBoxes)
    q.second->Remove(receiver);
}
