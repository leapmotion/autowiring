#pragma once
#include MEMORY_HEADER
#include <thread>

struct BasicThreadStateBlock:
  std::enable_shared_from_this<BasicThreadStateBlock>
{
  ~BasicThreadStateBlock();

  // General purpose thread lock and update condition for the lock
  std::mutex m_lock;
  std::condition_variable m_stateCondition;

  // The current thread, if running
  std::thread m_thisThread;
};

