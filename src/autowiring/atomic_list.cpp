// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "atomic_list.h"

using autowiring::callable_base;
using autowiring::atomic_list;

atomic_list::~atomic_list(void) {
  callable_base* next;
  for (callable_base* cur = m_pHead.load(std::memory_order_relaxed); cur; cur = next) {
    next = cur->m_pFlink;
    delete cur;
  }
}

uint32_t atomic_list::push_entry(callable_base* e) throw() {
  callable_base* pHead;
  uint32_t retVal;
  do {
    pHead = m_pHead.load(std::memory_order_acquire);
    retVal = m_chainID.load(std::memory_order_relaxed);
    e->m_pFlink = pHead;
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
