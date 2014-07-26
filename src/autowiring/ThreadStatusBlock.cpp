// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ThreadStatusBlock.h"
#include "GlobalCoreContext.h"
#include "thread_specific_ptr.h"

static autowiring::thread_specific_ptr<ThreadStatusBlock> s_statusBlock;

ThreadStatusBlock::ThreadStatusBlock(void):
  m_context(GlobalCoreContext::Get())
{}

ThreadStatusBlock::~ThreadStatusBlock(void){}

ThreadStatusBlock* ThreadStatusBlock::Get(void) {
  if(!s_statusBlock.get())
    // No current status block, prime one in:
    s_statusBlock.reset(new ThreadStatusBlock);

  return s_statusBlock.get();
}
