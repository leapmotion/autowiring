// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TeardownNotifier.h"

using namespace autowiring;

TeardownNotifier::TeardownNotifier(void)
{
  m_pFirstTeardownListener = nullptr;
}

TeardownNotifier::~TeardownNotifier(void) {
  // Notify all teardown listeners, this will be our last opportunity to do so:
  NotifyTeardownListeners();
}

void TeardownNotifier::NotifyTeardownListeners(void) {
  EntryBase* next;
  for (EntryBase* cur = m_pFirstTeardownListener; cur; cur = next) {
    next = cur->pFlink;
    (*cur)();
    delete cur;
  }

  m_pFirstTeardownListener = nullptr;
}

void TeardownNotifier::AddTeardownListenerInternal(EntryBase* listener) {
  EntryBase* priorHead = m_pFirstTeardownListener;
  while (!m_pFirstTeardownListener.compare_exchange_weak(priorHead, listener))
    listener->pFlink = priorHead;
}
