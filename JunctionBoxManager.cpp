// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "JunctionBoxManager.h"

JunctionBoxManager::JunctionBoxManager(void) {
  //TODO: Initialize m_junctionBoxes with all types in program
}

JunctionBoxManager::~JunctionBoxManager(void) {}

std::shared_ptr<JunctionBoxBase> JunctionBoxManager::Get(std::type_index pTypeIndex) {
  return m_junctionBoxes[pTypeIndex];
}

void JunctionBoxManager::ReleaseRefs(t_rcvrSet::iterator first, t_rcvrSet::iterator last){
  boost::lock_guard<boost::mutex> lk(m_lock);
  
  for(auto q = m_junctionBoxes.begin(); q != m_junctionBoxes.end(); q++) {
    q->second->ReleaseRefs(first, last);
  }
}

void JunctionBoxManager::RemoveSnoopers(t_rcvrSet::iterator first, t_rcvrSet::iterator last){
  boost::lock_guard<boost::mutex> lk(m_lock);
  
}

void JunctionBoxManager::AddEventReceiver(std::shared_ptr<EventReceiver> pRecvr){
  boost::lock_guard<boost::mutex> lk(m_lock);
  
  //Notify all currently used junctionboxs that there is a new event
  for(auto q = m_junctionBoxes.begin(); q != m_junctionBoxes.end(); q++)
    *q->second += pRecvr;
}

void JunctionBoxManager::RemoveEventReceiver(std::shared_ptr<EventReceiver> pRecvr){
  boost::lock_guard<boost::mutex> lk(m_lock);
  
  // Notify all compatible senders that we're going away:
  for(auto q = m_junctionBoxes.begin(); q != m_junctionBoxes.end(); q++)
    *(q->second) -= pRecvr;
}

void JunctionBoxManager::RemoveEventReceivers(t_rcvrSet::iterator first, t_rcvrSet::iterator last){
  boost::lock_guard<boost::mutex> lk(m_lock);
  
  for(auto r = m_junctionBoxes.begin(); r != m_junctionBoxes.end(); r++) {
    auto box = r->second;
    for(auto q = first; q != last; q++) {
      *box -= *q;
    }
  }
}
