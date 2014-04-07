#include "stdafx.h"
#include "CoreJob.h"
#include "CoreContext.h"
#include "move_only.h"
#include <future>

CoreJob::CoreJob(const char* name) :
  ContextMember(name),
  m_running(false)
{}

void CoreJob::FireEvent(DispatchThunkBase* thunk){
  MoveOnly<std::future<void>> prev(std::move(m_prevEvent));
  
  m_prevEvent = std::async(std::launch::async, [thunk, prev]{
    auto cleanup = MakeAtExit([thunk]{
      delete thunk;
    });
    
    // Wait for previous async to finish
    if (prev.value.valid()) prev.value.wait();
    (*thunk)();
  });
}

void CoreJob::OnPended(boost::unique_lock<boost::mutex>&& lk){
  lk.unlock();
  if (m_running) DispatchEvent();
}

bool CoreJob::IsRunning(void) const { return m_running; }

bool CoreJob::Start(std::shared_ptr<Object> outstanding) {
  std::shared_ptr<CoreContext> context = m_context.lock();
  if(!context)
    return false;
  
  m_outstanding = outstanding;
  m_running = true;
  DispatchAllEvents();
  
  m_jobUpdate.notify_all();
  
  return true;
}

void CoreJob::Stop(bool graceful) {
  
  if (graceful){
    // Pend a call which will invoke Abort once the dispatch queue is done:
    DispatchQueue::Pend([this] {
      this->m_running = false;
      this->Abort();
    });
  } else {
    // Abort the dispatch queue so anyone waiting will wake up
    DispatchQueue::Abort();
    m_running = false;
  }
  
  try {
    // If we are asked to rundown while we still have elements in our dispatch queue,
    // we must try to process them:
    DispatchAllEvents();
  }
  catch(...) {
    // We failed to run down the dispatch queue gracefully, we now need to abort it
    DispatchQueue::Abort();
  }
  
  m_outstanding.reset();
}

void CoreJob::Wait() {
  if (m_prevEvent.valid()) m_prevEvent.wait();
}
