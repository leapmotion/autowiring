// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "JunctionBoxManager.h"
#include "AutoPacketFactory.h"
#include "AutowiringEvents.h"
#include "JunctionBox.h"

JunctionBoxManager::JunctionBoxManager(void) {
  // Enumerate all event types to initialize a new JunctionBox for each
  for (auto p = g_pFirstEventEntry; p; p = p->pFlink)
    m_junctionBoxes[p->ti] = p->NewJunctionBox();
  
  // Always allow internal events
  m_junctionBoxes[typeid(AutowiringEvents)]->Initiate();
}

JunctionBoxManager::~JunctionBoxManager(void) {}

std::shared_ptr<JunctionBoxBase> JunctionBoxManager::Get(const std::type_index& pTypeIndex) {
  auto box = m_junctionBoxes.find(pTypeIndex);
  assert(box != m_junctionBoxes.end() && "If JunctionBox isn't found, EventRegistry isn't working");
  
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

void JunctionBoxManager::Initiate(void) {
  for (const auto& q : m_junctionBoxes)
    q.second->Initiate();
}

void JunctionBoxManager::AddEventReceiver(JunctionBoxEntry<Object> receiver) {
  // Notify all junctionboxes that there is a new event
  for(const auto& q: m_junctionBoxes)
    q.second->Add(receiver);
}

void JunctionBoxManager::RemoveEventReceiver(JunctionBoxEntry<Object> receiver) {
  // Notify all compatible senders that we're going away:
  for(const auto& q : m_junctionBoxes)
    q.second->Remove(receiver);
}

bool JunctionBoxManager::CheckEventOutputStream(const std::type_index& type){
  auto mapfinditerator = m_eventOutputStreams.find(type);
  if(mapfinditerator != m_eventOutputStreams.end()) {
    auto v = (mapfinditerator->second);
    auto it = v.begin();
    while(it != v.end()) {
      if((*it).lock())
        return true;
      it = v.erase(it);
    }
    return false; //return false if iterated through whole vec without seeing any live pointers.
  }
  return false;  //return false if no vec with that type
}

void JunctionBoxManager::AddEventOutputStream(const std::type_index& type, std::weak_ptr<EventOutputStreamBase> pRecvr){
  auto mapfinditerator = m_eventOutputStreams.find(type);
  if (mapfinditerator != m_eventOutputStreams.end()){
    // If the type exists already, find the correspoonding outputstreambase and push it back.
    mapfinditerator->second.push_back(pRecvr);
  }
  else {
    std::vector<std::weak_ptr<EventOutputStreamBase> > newvec;
    newvec.push_back(pRecvr);
    m_eventOutputStreams[type] = newvec; //assignment copy constructor invoked;
  }
}
