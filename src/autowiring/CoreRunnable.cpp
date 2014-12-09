// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "CoreRunnable.h"

CoreRunnable::CoreRunnable(void):
m_wasStarted(false),
  m_shouldStop(false)
{}

CoreRunnable::~CoreRunnable(void){}

void CoreRunnable::Start(std::shared_ptr<Object> outstanding) {
  if(m_outstanding || m_wasStarted)
    // We have already been started, end here
    return;

  m_wasStarted = true;
  if(!DoStart()) {
    m_shouldStop = true;
    return;
  }

  m_outstanding = outstanding;
}