// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include MUTEX_HEADER

struct CoreContextStateBlock
{
public:
  CoreContextStateBlock(void);
  ~CoreContextStateBlock(void);

  // General purpose lock for this class
  std::mutex m_lock;

  // Condition, signalled when context state has been changed
  std::condition_variable m_stateChanged;
};

