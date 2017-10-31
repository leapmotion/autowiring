// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include "CoreObject.h"
#include <memory>
#include MUTEX_HEADER

class CoreContext;

namespace autowiring {

struct CoreContextStateBlock;

/// <summary>
/// Container type used for state block referencing
/// </summary>
class RunCounter:
  public CoreObject
{
public:
  RunCounter(const std::shared_ptr<CoreContextStateBlock>& stateBlock, const std::shared_ptr<CoreContext>& owner);
  ~RunCounter(void);

private:
  const std::shared_ptr<CoreContextStateBlock> stateBlock;
  const std::shared_ptr<RunCounter> parentCount;
  std::shared_ptr<CoreContext> owner;
};

struct CoreContextStateBlock:
  std::enable_shared_from_this<CoreContextStateBlock>
{
public:
  CoreContextStateBlock(std::shared_ptr<CoreContextStateBlock> parent);
  ~CoreContextStateBlock(void);

  // Reference to the parent state block, where appropriate
  const std::shared_ptr<CoreContextStateBlock> parent;

  // General purpose lock for this class
  std::mutex m_lock;

  // Condition, signalled when context state has been changed
  std::condition_variable m_stateChanged;

  // Clever use of shared pointer to expose the number of outstanding CoreRunnable instances.
  // Destructor does nothing; this is by design.
  std::weak_ptr<autowiring::RunCounter> m_outstanding;

  /// \internal
  /// <summary>
  /// Increments the total number of contexts still outstanding
  /// </summary>
  /// <param name="owner">The context that will be held for as long as the outstanding count is valid</param>
  /// <remarks>
  /// This is an indirect incrementation routine.  The count will be incremented for as
  /// long as the returned shared_ptr is not destroyed.  Once it's destroyed, the count
  /// is decremented.  The caller is encouraged not to copy the return value, as doing
  /// so can give inflated values for the current number of outstanding threads.
  ///
  /// The caller is responsible for exterior synchronization
  /// </remarks>
  std::shared_ptr<autowiring::RunCounter> IncrementOutstandingThreadCount(std::shared_ptr<CoreContext> owner);
};

}
