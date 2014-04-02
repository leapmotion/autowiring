// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "CoreThread.h"
#include "Autowired.h"
#include <boost/thread.hpp>

CoreThread::CoreThread(const char* pName):
  BasicThread(pName),
  m_canAccept(false)
{}

void CoreThread::DoRun(void) {
  assert(m_running);

  // Make our own session current before we do anything else:
  CurrentContextPusher pusher(GetContext());

  // Set the thread name no matter what:
  if(GetName())
    SetCurrentThreadName();

  // Now we wait for the thread to be good to go:
  try {
    Run();
  } catch(dispatch_aborted_exception&) {
    // Okay, this is fine, a dispatcher is terminating--this is a normal way to
    // end a dispatcher loop.
  } catch(...) {
    try {
      // Ask that the enclosing context filter this exception, if possible:
      GetContext()->FilterException();
    } catch(...) {
      // Generic exception, unhandled, we can't do anything about this
    }

    // Signal shutdown on the enclosing context--cannot wait, if we wait we WILL deadlock
    GetContext()->SignalShutdown(false);
  }

  // Unconditionally shut off dispatch delivery:
  RejectDispatchDelivery();

  try {
    // If we are asked to rundown while we still have elements in our dispatch queue,
    // we must try to process them:
    DispatchAllEvents();
  }
  catch(...) {
    // We failed to run down the dispatch queue gracefully, we now need to abort it
    Abort();
  }

  // Notify everyone that we're completed:
  boost::lock_guard<boost::mutex> lk(m_lock);
  m_stop = true;
  m_completed = true;
  m_running = false;

  // Perform a manual notification of teardown listeners
  NotifyTeardownListeners();

  // No longer running, we MUST release the thread pointer to ensure proper teardown order
  m_thisThread.detach();

  // Take a copy of our state condition shared pointer while we still hold a reference to
  // ourselves.  This is the only member out of our collection of members that we actually
  // need to hold a reference to.
  auto state = m_state;

  // Release our hold on the context.  After this point, we have to be VERY CAREFUL that we
  // don't try to refer to any of our own member variables, because our own object may have
  // already gone out of scope.  [this] is potentially dangling.
  pusher.Pop();

  // Notify other threads that we are done.  At this point, any held references that might
  // still exist are held by entities other than ourselves.
  state->m_stateCondition.notify_all();
}

bool CoreThread::DelayUntilCanAccept(void) {
  boost::unique_lock<boost::mutex> lk(m_lock);
  m_stateCondition.wait(lk, [this] {return ShouldStop() || CanAccept(); });
  return !ShouldStop();
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

void CoreThread::Stop(bool graceful){
  // Trivial return check:
  if(m_stop)
    return;
  
  // Perform initial stop:
  m_stop = true;
  OnStop();
  
  if(graceful) {
    // Signal the dispatch queue to run down
    RejectDispatchDelivery();
    
    // Pend a call which will invoke Abort once the dispatch queue is done:
    DispatchQueue::Pend([this] {
      this->Abort();
      
      // Notify callers of our new state:
      this->m_state->m_stateCondition.notify_all();
    });
  } else {
    // Abort the dispatch queue so anyone waiting will wake up
    DispatchQueue::Abort();
    
    // Notify all callers of our status update, only needed if we don't call
    // RejectDispatchDelivery first
    m_state->m_stateCondition.notify_all();
  }
}

void CoreThread::Run() {
  AcceptDispatchDelivery();

  while(!ShouldStop())
    WaitForEvent();
}
