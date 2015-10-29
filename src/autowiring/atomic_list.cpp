// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "atomic_list.h"

using autowiring::atomic_entry;
using autowiring::atomic_list;

atomic_list::~atomic_list(void) {
  atomic_entry* next;
  for (atomic_entry* cur = m_pHead.load(std::memory_order_relaxed); cur; cur = next) {
    next = cur->pFlink;
    delete cur;
  }
}

uint32_t atomic_list::push_entry(atomic_entry* e) throw() {
  atomic_entry* pHead;
  uint32_t retVal;
  do {
    pHead = m_pHead.load(std::memory_order_acquire);
    retVal = m_chainID.load(std::memory_order_relaxed);
    e->pFlink = pHead;
  } while (
    !m_pHead.compare_exchange_weak(
      pHead,
      e,
      std::memory_order_release,
      std::memory_order_relaxed
    )
  );
  return retVal;
}
