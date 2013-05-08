#include "stdafx.h"
#include "ThreadStatusMaintainer.h"
#include "CoreContext.h"
#include "CoreThread.h"

ThreadStatusMaintainer::ThreadStatusMaintainer(CoreThread* pThread, const std::shared_ptr<CoreContext>& context):
  pThread(pThread),
  context(context),
  outstanding(context->IncrementOutstandingThreadCount())
{
}

void ThreadStatusMaintainer::operator()() {
  pThread->DoRun();
}