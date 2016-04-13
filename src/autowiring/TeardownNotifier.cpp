// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TeardownNotifier.h"
#include "InterlockedExchange.h"

using namespace autowiring;

TeardownNotifier::TeardownNotifier(void)
{}

TeardownNotifier::~TeardownNotifier(void) {
  // Notify all teardown listeners, this will be our last opportunity to do so:
  NotifyTeardownListeners();
}

void TeardownNotifier::NotifyTeardownListeners(void) {
  EntryBase* next;
  for (auto cur = m_pFirstTeardownListener; cur; cur = next) {
    next = cur->pFlink;
    (*cur)();
    delete cur;
  }

  m_pFirstTeardownListener = nullptr;
}

void TeardownNotifier::AddTeardownListenerInternal(EntryBase* listener) {
  EntryBase* pResult;
  do {
    listener->pFlink = m_pFirstTeardownListener;
    pResult = compare_exchange<EntryBase>(&m_pFirstTeardownListener, listener, listener->pFlink);
  } while (pResult != listener->pFlink);
}
