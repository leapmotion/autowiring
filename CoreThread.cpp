#include "stdafx.h"
#include "CoreThread.h"
#include "Autowired.h"
#include "BasicThreadStateBlock.h"
#include <boost/thread.hpp>

CoreThread::CoreThread(const char* pName):
  BasicThread(pName)
{}

void CoreThread::DoRunLoopCleanup(std::shared_ptr<CoreContext>&& ctxt, std::shared_ptr<Object>&& refTracker) {
  try {
    // If we are asked to rundown while we still have elements in our dispatch queue,
    // we must try to process them:
    CurrentContextPusher pshr(ctxt);
    DispatchAllEvents();
  }
  catch(...) {
    // We failed to run down the dispatch queue gracefully, we now need to abort it
    Abort();
  }

  // Handoff to base class:
  BasicThread::DoRunLoopCleanup(std::move(ctxt), std::move(refTracker));
}

void CoreThread::WaitForEvent(void) {
  boost::unique_lock<boost::mutex> lk(m_dispatchLock);
  if(m_aborted)
    throw dispatch_aborted_exception();

  // Unconditional delay:
  m_queueUpdated.wait(lk, [this] () -> bool {
    if(m_aborted)
      throw dispatch_aborted_exception();

    return
    // We will need to transition out if the delay queue receives any items:
    !this->m_delayedQueue.empty() ||

    // We also transition out if the dispatch queue has any events:
    !this->m_dispatchQueue.empty();
  });

  if(m_dispatchQueue.empty())
    // The delay queue has items but the dispatch queue does not, we need to switch
    // to the suggested sleep timeout variant:
    WaitForEventUnsafe(lk, m_delayedQueue.top().GetReadyTime());
  else
    // We have an event, we can just hop over to this variant:
    DispatchEventUnsafe(lk);
}

bool CoreThread::WaitForEvent(boost::chrono::milliseconds milliseconds) {
  return WaitForEvent(boost::chrono::high_resolution_clock::now() + milliseconds);
}

bool CoreThread::WaitForEvent(boost::chrono::high_resolution_clock::time_point wakeTime) {
  if(wakeTime == boost::chrono::steady_clock::time_point::max())
    // Maximal wait--we can optimize by using the zero-arguments version
    return WaitForEvent(), true;

  boost::unique_lock<boost::mutex> lk(m_dispatchLock);
  return WaitForEventUnsafe(lk, wakeTime);
}

bool CoreThread::WaitForEventUnsafe(boost::unique_lock<boost::mutex>& lk, boost::chrono::high_resolution_clock::time_point wakeTime) {
  if(m_aborted)
    throw dispatch_aborted_exception();

  while(m_dispatchQueue.empty()) {
    // Derive a wakeup time using the high precision timer:
    wakeTime = SuggestSoonestWakeupTimeUnsafe(wakeTime);

    // Now we wait, either for the timeout to elapse or for the dispatch queue itself to
    // transition to the "aborted" state.
    boost::cv_status status = m_queueUpdated.wait_until(lk, wakeTime);

    // Short-circuit if the queue was aborted
    if(m_aborted)
      throw dispatch_aborted_exception();

    // Pull over any ready events:
    PromoteReadyEventsUnsafe();

    // Dispatch events if the queue is now non-empty:
    if(!m_dispatchQueue.empty())
      break;

    if(status == boost::cv_status::timeout)
      // Can't proceed, queue is empty and nobody is ready to be run
      return false;
  }

  DispatchEventUnsafe(lk);
  return true;
}

void CoreThread::Run() {
  while(!ShouldStop())
    WaitForEvent();
}

void CoreThread::Stop(bool graceful) {
  if(graceful) {
    // Pend a call which will invoke Abort once the dispatch queue is done:
    DispatchQueue::Pend([this] {
      this->Abort();

      // Notify callers of our new state:
      this->m_state->m_stateCondition.notify_all();
    });
  } else
    // Abort the dispatch queue so anyone waiting will wake up
    DispatchQueue::Abort();

  // Pass off to base class handling:
  BasicThread::Stop(graceful);
}

double CoreThread::GetThreadUtilization(){
  return double(rand()%100);
}