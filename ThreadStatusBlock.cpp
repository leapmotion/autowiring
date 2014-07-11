#include "stdafx.h"
#include "ThreadStatusBlock.h"
#include "GlobalCoreContext.h"
#include <boost/thread/tss.hpp>

static boost::thread_specific_ptr<ThreadStatusBlock> s_statusBlock;

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
