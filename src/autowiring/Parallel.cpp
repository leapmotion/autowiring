// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "Parallel.h"
#include "autowiring.h"
#include <thread>

using namespace autowiring;

void parallel_iterator<void>::operator++(int) {
  m_parent.Pop<void>();
}

parallel::parallel(void):
  parallel{ *CoreContext::CurrentContext(), std::thread::hardware_concurrency() }
{}

parallel::parallel(size_t concurrency) :
  parallel{ *CoreContext::CurrentContext(), concurrency }
{}

parallel::parallel(CoreContext& ctxt, size_t concurrency):
  m_ctxt(ctxt.shared_from_this())
{
  if (!concurrency)
    concurrency = std::thread::hardware_concurrency();

  auto block = m_block;

  // Fire off a bunch of threads to do work:
  while (concurrency--)
    std::thread(
      [block] {
        while(block->owned)
          try { block->dq.WaitForEvent(); }
          catch(dispatch_aborted_exception&) {
            // Expected behavior, things tearing down, end here
            return;
          }
      }
    ).detach();

  // Configure our signal after everything else is done
  onStopReg = ctxt.onShutdown += [this, block] {
    std::lock_guard<std::mutex> lk(block->m_lock);
    if (!block->owned)
      // Status block already destroyed, we're in a teardown race
      // Short-circuit here, no reason to double-call stop
      return;

    stop_unsafe();
  };
}

parallel::~parallel(void) {
  stop();
}

void parallel::stop(void) {
  std::lock_guard<std::mutex> lk(m_block->m_lock);
  stop_unsafe();
}

void parallel::stop_unsafe(void) {
  // Trivial return check:
  if (!m_block)
    return;

  m_block->owned = false;
  m_ctxt->onShutdown -= onStopReg;
  m_ctxt.reset();
  m_block.reset();
}
