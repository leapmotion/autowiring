#include "stdafx.h"
#include "CoreJob.h"
#include "CoreContext.h"
#include "move_only.h"

CoreJob::CoreJob(const char* name) :
  ContextMember(name),
  m_running(false),
  m_shouldStop(false)
{}

void CoreJob::OnPended(boost::unique_lock<boost::mutex>&& lk){
  if(m_curEvent.valid())
    // Something is already outstanding, it will handle dispatching
    // again for us.
    return;

  // Need to ask the thread pool to handle our events again:
  m_curEvent = std::async(
    std::launch::async,
    [this] { this->DispatchAllAndClearCurrent(); }
  );
}

void CoreJob::DispatchAllAndClearCurrent(void) {
  for(;;) {
    // Trivially run down the queue as long as we're in the pool:
    this->DispatchAllEvents();

    // Check the size of the queue.  Could be that someone added something
    // between when we finished looping, and when we obtained the lock, and
    // we don't want to exit our pool if that has happened.
    boost::lock_guard<boost::mutex> lk(m_dispatchLock);
    if(AreAnyDispatchersReady())
      continue;

    // Now that we've checked while holding the lock, we can reset our future
    // event and leave our async.  The next thread to check curEvent will have
    // to do so only while holding the lock we've currently got.
    m_curEvent = std::future<void>();
    return;
  }
}

bool CoreJob::Start(std::shared_ptr<Object> outstanding) {
  std::shared_ptr<CoreContext> context = m_context.lock();
  if(!context)
    return false;
  
  m_outstanding = outstanding;
  m_running = true;
  DispatchEvent();
  
  return true;
}

void CoreJob::Abort(void) {
  DispatchQueue::Abort();
  m_running = false;
  m_outstanding.reset();
}

void CoreJob::Stop(bool graceful) {
  if(graceful)
    // Pend a call which will invoke Abort once the dispatch queue is done:
    DispatchQueue::Pend([this] { this->Abort(); });
  else
    // Abort the dispatch queue so anyone waiting will wake up
    Abort();

  // Hit our condition variable to wake up any listeners:
  boost::lock_guard<boost::mutex> lk(m_dispatchLock);
  m_shouldStop = true;
  m_queueUpdated.notify_all();
}

void CoreJob::Wait() {
  // Condition variable strike:
  boost::unique_lock<boost::mutex> lk(m_dispatchLock);
  m_queueUpdated.wait(
    lk,
    [this] { return ShouldStop(); }
  );
}
