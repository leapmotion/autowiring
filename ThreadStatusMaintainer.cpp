#include "stdafx.h"
#include "ThreadStatusMaintainer.h"
#include "CoreContext.h"
#include "CoreThread.h"

ThreadStatusMaintainer::ThreadStatusMaintainer(CoreThread* pThread, const cpp11::shared_ptr<CoreContext>& context):
    pThread(pThread),
    context(context),
    outstanding(context->IncrementOutstandingThreadCount())
{
}

ThreadStatusMaintainer::~ThreadStatusMaintainer(void) {
  pThread->m_running = false;
}

void ThreadStatusMaintainer::operator()() {
  pThread->DelayUntilReady();
  pThread->m_running = true;
  pThread->Run();
}