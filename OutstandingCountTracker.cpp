// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "OutstandingCountTracker.h"
#include "CoreContext.h"

OutstandingCountTracker::OutstandingCountTracker(std::shared_ptr<CoreContext> context):
  m_context(context)
{
}

OutstandingCountTracker::~OutstandingCountTracker(void) {
  boost::lock_guard<boost::mutex> lk(m_context->m_lock);
  m_context->m_stateChanged.notify_all();
}
