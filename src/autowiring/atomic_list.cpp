// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "atomic_list.h"
#include <mutex>

using autowiring::callable_base;
using autowiring::atomic_list;

atomic_list::~atomic_list(void) {
  callable_base* next;
  while(m_pHead) {
    next = m_pHead->m_pFlink;
    delete m_pHead;
  }
}

uint32_t atomic_list::push_entry(callable_base* e) throw() {
  std::lock_guard<autowiring::spin_lock> lk(lock);
  e->m_pFlink = m_pHead;
  m_pHead = e;
  return m_chainID;
}

callable_base* atomic_list::release(void) throw() {
  callable_base* retVal = nullptr;
  callable_base* pHead = nullptr;

  // Extract the list head and increment the chain identifier for the next customer
  {
    std::lock_guard<autowiring::spin_lock> lk(lock);
    m_chainID++;
    pHead = m_pHead;
    m_pHead = nullptr;
  }

  // Flip the links around, append to return collection in order
  for (callable_base* pNext; pHead; pHead = pNext) {
    // Record the next pointer before we nullify it
    pNext = pHead->m_pFlink;

    // Append to the return list:
    pHead->m_pFlink = retVal;
    retVal = pHead;
  }
  return retVal;
}
