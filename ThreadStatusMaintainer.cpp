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

ThreadStatusMaintainer::~ThreadStatusMaintainer(void) {
  pThread->m_running = false;
}

void ThreadStatusMaintainer::operator()() {
  // Make our own session current before we do anything else:
  CurrentContextPusher pusher(pThread->GetContext());

  // Now we wait for the thread to be good to go:
  pThread->DelayUntilReady();
  pThread->Run();

  // Notify everyone that we're completed:
  boost::lock_guard<boost::mutex>(pThread->m_lock),
  pThread->m_completed = true,
  pThread->m_completionCondition.notify_all();
}