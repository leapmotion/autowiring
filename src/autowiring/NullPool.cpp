// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "NullPool.h"

using namespace autowiring;

NullPool::NullPool(void)
{}

NullPool::~NullPool(void)
{}

void NullPool::SetSuccessor(const std::shared_ptr<ThreadPool>& successor) {
  std::lock_guard<std::mutex> lk(m_lock);
  m_successor = successor;
}

std::shared_ptr<ThreadPool> NullPool::MoveDispatchersToSuccessor(void) {
  std::shared_ptr<ThreadPool> successor;
  std::vector<std::unique_ptr<DispatchThunkBase>> dispatchers;

  // Transfer over so we hold ownership, then release the lock
  {
    std::lock_guard<std::mutex> lk(m_lock);

    // If there is no successor then we have to return here, we can't tear
    // down lambdas safely from here
    if (!m_successor)
      return nullptr;

    successor = std::move(m_successor);
    m_successor.reset();
    dispatchers = std::move(m_dispatchers);
    m_dispatchers.clear();
  }

  for (auto& dispatcher : dispatchers)
    successor->Submit(std::move(dispatcher));
  return successor;
}

std::shared_ptr<void> NullPool::Start(void) {
  auto successor = GetSuccessor();
  if (!successor)
    return nullptr;
  return successor->Start();
}

bool NullPool::Submit(std::unique_ptr<DispatchThunkBase>&& thunk) {
  m_dispatchers.push_back(std::move(thunk));
  return true;
}
