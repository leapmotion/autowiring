#include "stdafx.h"
#include "CoreJob.h"
#include "CoreContext.h"
#include "move_only.h"

CoreJob::CoreJob(const char* name) :
  ContextMember(name),
  m_running(false),
  m_shouldStop(false),
  m_curEventInTeardown(true)
{}

void CoreJob::OnPended(std::unique_lock<std::mutex>&& lk){
  if(!m_curEventInTeardown)
    // Something is already outstanding, it will handle dispatching for us.
    return;

  if(!m_running)
    // Nothing to do, we aren't running yet--just hold on to this entry until we are
    // ready to initiate it.
    return;

	// Increment outstanding count because we now have an entry out in a thread pool
	auto outstanding = m_outstanding;

	if(!outstanding)
		// We're currently signalled to stop, we must empty the queue and then
		// return here--we can't accept dispatch delivery on a stopped queue.
		while(!m_dispatchQueue.empty()) {
			delete m_dispatchQueue.front();
			m_dispatchQueue.pop_front();
		}
	else {
		// Need to ask the thread pool to handle our events again:
		m_curEventInTeardown = false;
		m_curEvent = std::async(
			std::launch::async,
			[this, outstanding] () mutable {
				this->DispatchAllAndClearCurrent();
				outstanding.reset();
			}
		);
	}
}

void CoreJob::DispatchAllAndClearCurrent(void) {
  CurrentContextPusher pshr(GetContext());
  for(;;) {
    // Trivially run down the queue as long as we're in the pool:
    this->DispatchAllEvents();

    // Check the size of the queue.  Could be that someone added something
    // between when we finished looping, and when we obtained the lock, and
    // we don't want to exit our pool if that has happened.
    std::lock_guard<std::mutex> lk(m_dispatchLock);
		if(AreAnyDispatchersReady())
			continue;

		// Indicate that we're tearing down and will be done very soon.  This is
		// a signal to consumers that a call to m_curEvent.wait() will be nearly
		// non-blocking.
		m_curEventInTeardown = true;
    m_queueUpdated.notify_all();
    break;
  }
}

bool CoreJob::Start(std::shared_ptr<Object> outstanding) {
  std::shared_ptr<CoreContext> context = m_context.lock();
  if(!context)
    return false;

  m_outstanding = outstanding;
  m_running = true;

  std::unique_lock<std::mutex> lk;
  if(!m_dispatchQueue.empty())
	  // Simulate a pending event, because we need to set up our async:
	  OnPended(std::move(lk));

  return true;
}

void CoreJob::Abort(void) {
  DispatchQueue::Abort();
  m_running = false;
}

void CoreJob::Stop(bool graceful) {
  if(graceful)
    // Pend a call which will invoke Abort once the dispatch queue is done:
    DispatchQueue::Pend(
			[this] {this->Abort();}
		);
	else
    // Abort the dispatch queue so anyone waiting will wake up
    Abort();

	// Reset the outstanding pointer, we don't intend to hold it anymore:
	m_outstanding.reset();

  // Hit our condition variable to wake up any listeners:
  std::lock_guard<std::mutex> lk(m_dispatchLock);
  m_shouldStop = true;
  m_queueUpdated.notify_all();
}

void CoreJob::Wait() {
  {
    std::unique_lock<std::mutex> lk(m_dispatchLock);
    m_queueUpdated.wait(
      lk,
      [this] {
        return ShouldStop() && m_curEventInTeardown;
      }
    );
  }

  // If the current event is valid, we can block on it until it becomes valid:
  if(m_curEvent.valid())
    m_curEvent.wait();
}

bool CoreJob::WaitFor(std::chrono::nanoseconds duration) {
  {
    std::unique_lock<std::mutex> lk(m_dispatchLock);
    if(!m_queueUpdated.wait_for(
      lk,
      duration,
      [this] {
        return ShouldStop() && m_curEventInTeardown;
      }
    ))
      return false;
  }

  // If the current event is valid, we can block on it until it becomes valid:
  if(m_curEvent.valid())
    m_curEvent.wait();
  return true;
}