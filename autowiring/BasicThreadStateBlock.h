// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include MEMORY_HEADER
#include THREAD_HEADER
#include MUTEX_HEADER

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
