// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TeardownNotifier.h"

TeardownNotifier::~TeardownNotifier(void) {
  // Notify all teardown listeners, this will be our last opportunity to do so:
  NotifyTeardownListeners();
}

void TeardownNotifier::NotifyTeardownListeners(void) {
  for(auto q = m_teardownListeners.begin(); q != m_teardownListeners.end(); q++)
    (*q)();
  m_teardownListeners.clear();
}

void TeardownNotifier::AddTeardownListener(const std::function<void ()>& listener) {
  m_teardownListeners.push_back(listener);
}