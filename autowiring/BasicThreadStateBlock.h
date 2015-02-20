// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include MEMORY_HEADER
#include MUTEX_HEADER
#include THREAD_HEADER

struct BasicThreadStateBlock:
  std::enable_shared_from_this<BasicThreadStateBlock>
{
  BasicThreadStateBlock(void);
  ~BasicThreadStateBlock(void);

  // General purpose thread lock and update condition for the lock
  std::mutex m_lock;
  std::condition_variable m_stateCondition;

  // The current thread, if running
  std::thread m_thisThread;

  // Completion condition, true when this thread is no longer running and has run at least once
  bool m_completed;
};
