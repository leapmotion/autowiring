// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include "ThreadPool.h"
#include "DispatchQueue.h"

namespace autowiring {

/// <summary>
/// A placeholder pool that accumulates work items until the owning context is started
/// </summary>
/// <remarks>
/// This pool type should not be used directly
/// </remarks>
class NullPool:
  public ThreadPool
{
public:
  NullPool(void);
  ~NullPool(void);

private:
  std::vector<std::unique_ptr<DispatchThunkBase>> m_dispatchers;

  // This thread pool will be replaced by the specified successor pool when the
  // owning context is started.  If this member is null, the parent context's
  // thread pool is taken instead.
  std::shared_ptr<ThreadPool> m_successor;

public:
  std::shared_ptr<ThreadPool> GetSuccessor(void) const { return m_successor; }

  /// <summary>
  /// Sets the pool that will be used by the owning context when the owning context initiates
  /// </summary>
  void SetSuccessor(const std::shared_ptr<ThreadPool>& successor);

  /// <summary>
  /// Causes all internally held dispatchers to be moved to the successor pool, then clears this pool
  /// </summary>
  /// <returns>The populated successor</returns>
  std::shared_ptr<ThreadPool> MoveDispatchersToSuccessor(void);

  // ThreadPool overrides:
  std::shared_ptr<void> Start(void) override;
  bool Submit(std::unique_ptr<DispatchThunkBase>&& thunk) override;
};

}
