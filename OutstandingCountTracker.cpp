#include "stdafx.h"
#include "OutstandingCountTracker.h"
#include "CoreContext.h"

OutstandingCountTracker::OutstandingCountTracker(cpp11::shared_ptr<CoreContext> context):
  m_context(context)
{
}

OutstandingCountTracker::~OutstandingCountTracker(void) {
  m_context->m_stop.notify_all();
}