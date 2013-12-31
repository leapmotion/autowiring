// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "TypeRegistry.h"
#include "JunctionBoxManager.h"
#include "AutoPacketFactory.h"
#include "AutoPacketListener.h"

JunctionBoxManager::JunctionBoxManager(void) {
  
  for(auto p = g_pFirstEntry; p; p = p->pFlink) {
    m_junctionBoxes[p->ti] = p->m_NewJunctionBox();
  }
  
  m_junctionBoxes[typeid(AutoPacketListener)] = std::make_shared<JunctionBox<AutoPacketListener>>();
  
}

JunctionBoxManager::~JunctionBoxManager(void) {
  for(auto q = m_junctionBoxes.begin(); q != m_junctionBoxes.end(); q++) {
    q->second.reset();
  }
}

std::shared_ptr<JunctionBoxBase> JunctionBoxManager::Get(std::type_index pTypeIndex) {
  return m_junctionBoxes[pTypeIndex];
}

void JunctionBoxManager::ReleaseRefs(t_rcvrSet::iterator first, t_rcvrSet::iterator last){
  boost::lock_guard<boost::mutex> lk(m_lock);
  
  for(auto q = m_junctionBoxes.begin(); q != m_junctionBoxes.end(); q++) {
    auto box = q->second;
    box->ReleaseRefs(first, last);
  }
}

void JunctionBoxManager::RemoveSnoopers(t_rcvrSet::iterator first, t_rcvrSet::iterator last){
  boost::lock_guard<boost::mutex> lk(m_lock);
  
  for(auto r = m_junctionBoxes.begin(); r != m_junctionBoxes.end(); r++) {
    auto box = r->second;
    for(auto q = first; q != last; q++) {
      *box -= *q;
    }
  }
}

void JunctionBoxManager::AddEventReceiver(std::shared_ptr<EventReceiver> pRecvr){
  boost::lock_guard<boost::mutex> lk(m_lock);
  
  //Notify all currently used junctionboxes that there is a new event
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
