// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once
#include MEMORY_HEADER
#include MUTEX_HEADER
#include THREAD_HEADER

enum class ThreadPriority;

namespace autowiring {

struct BasicThreadStateBlock:
  std::enable_shared_from_this<BasicThreadStateBlock>
{
  BasicThreadStateBlock(void);
  ~BasicThreadStateBlock(void);

  // Lock used to protect the actual thread
  std::mutex m_threadLock;

  // General purpose thread lock and update condition for the lock
  std::mutex m_lock;
  std::condition_variable m_stateCondition;

  // The current thread, if running
  std::thread m_thisThread;

  ThreadPriority m_priority;

  // Completion condition, true when this thread is no longer running and has run at least once
  bool m_completed = false;
};

}
