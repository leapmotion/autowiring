// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "GlobalCoreContext.h"
#include <cassert>

GlobalCoreContext::GlobalCoreContext(void):
  CoreContextT<GlobalCoreContext>(nullptr, t_childList::iterator())
{
  // Guard against multi-initialization:
  assert(!getGlobalContextSharedPtr());

  // Set up the global shared pointer:
  getGlobalContextSharedPtr().reset(this);

  // Make ourselves the current context before filling it:
  SetCurrent();
}

GlobalCoreContext::~GlobalCoreContext(void) {}

// Obtains the currently generated global context:
std::shared_ptr<GlobalCoreContext> GlobalCoreContext::Get() {
  if(getGlobalContextSharedPtr())
    return getGlobalContextSharedPtr();

  std::lock_guard<std::mutex> lk(getInitLock());
  if(getGlobalContextSharedPtr())
    // Multi-init by another thread, just short-circuit here
    return getGlobalContextSharedPtr();

  // Create a new core context.  Constructor will automatically set the shared pointer.
  GlobalCoreContext* pPtr = new GlobalCoreContext();
  assert(getGlobalContextSharedPtr());
  assert(getGlobalContextSharedPtr().get() == pPtr);

  // Unreferenced, force a ref here to avoid complaints.
  (void)pPtr;

  // We return a copy of getGlobalContextSharedPtr() to prevent the principal from being modified
  // due to rvalue optimization.
  std::shared_ptr<GlobalCoreContext> ptr(getGlobalContextSharedPtr());
  return ptr;
}

std::shared_ptr<GlobalCoreContext> GlobalCoreContext::Release(void) {
  // Release local:
  std::lock_guard<std::mutex> lk(getInitLock());
  auto retVal = getGlobalContextSharedPtr();
  getGlobalContextSharedPtr().reset();
  return retVal;
}

std::mutex& GlobalCoreContext::getInitLock() {
  static std::mutex s_initLock;
  return s_initLock;
}

std::shared_ptr<GlobalCoreContext>& GlobalCoreContext::getGlobalContextSharedPtr() {
  static std::shared_ptr<GlobalCoreContext> s_globalContextSharedPtr;
  return s_globalContextSharedPtr;
}

std::shared_ptr<GlobalCoreContext> GetGlobalContext(void) {
  return GlobalCoreContext::Get();
}
