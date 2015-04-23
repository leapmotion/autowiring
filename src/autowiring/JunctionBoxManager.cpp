// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "JunctionBoxManager.h"
#include "AutoPacketFactory.h"
#include "AutowiringEvents.h"
#include "demangle.h"
#include "JunctionBox.h"
#include "EventRegistry.h"
#include <sstream>

using namespace autowiring;

template class RegEvent<AutowiringEvents>;
template class TypeUnifierComplex<AutowiringEvents>;

JunctionBoxManager::JunctionBoxManager(void) {
  // Enumerate all event types to initialize a new JunctionBox for each
  for (auto p = g_pFirstEventEntry; p; p = p->pFlink)
    m_junctionBoxes[p->ti] = p->NewJunctionBox();

  // Make sure AutowiringEvents is in EventRegistry
  m_junctionBoxes[typeid(AutowiringEvents)] = std::make_shared<JunctionBox<AutowiringEvents>>();
  
  // Always allow internal events
  m_junctionBoxes[typeid(AutowiringEvents)]->Initiate();
}

JunctionBoxManager::~JunctionBoxManager(void) {}

std::shared_ptr<JunctionBoxBase> JunctionBoxManager::Get(const std::type_index& pTypeIndex) {
  auto box = m_junctionBoxes.find(pTypeIndex);
  if (box == m_junctionBoxes.end()) {
    std::ostringstream ss;
    ss << "Attempted to obtain a junction box for [[ " << demangle(pTypeIndex) << " ]], which is not registered." << std::endl
       << "Known junction boxes:" << std::endl;
    for (const auto& cur : m_junctionBoxes)
      ss << demangle(cur.first) << std::endl;
    throw autowiring_error(ss.str());
  }

  return box->second;
}

void JunctionBoxManager::Initiate(void) {
  for (const auto& q : m_junctionBoxes)
    q.second->Initiate();
}

void JunctionBoxManager::AddEventReceiver(JunctionBoxEntry<CoreObject> receiver) {
  // Notify all junctionboxes that there is a new event
  for(const auto& q: m_junctionBoxes)
    q.second->Add(receiver);
}

void JunctionBoxManager::RemoveEventReceiver(JunctionBoxEntry<CoreObject> receiver) {
  // Notify all compatible senders that we're going away:
  for(const auto& q : m_junctionBoxes)
    q.second->Remove(receiver);
}
