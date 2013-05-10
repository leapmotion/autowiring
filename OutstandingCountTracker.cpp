#include "stdafx.h"
#include "OutstandingCountTracker.h"
#include "CoreContext.h"

OutstandingCountTracker::OutstandingCountTracker(std::shared_ptr<CoreContext> context):
  m_context(context)
{
}

OutstandingCountTracker::~OutstandingCountTracker(void) {
  m_context->m_stop.notify_all();
}
