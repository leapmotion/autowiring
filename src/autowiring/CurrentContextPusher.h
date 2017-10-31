// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include MEMORY_HEADER

class CoreContext;
class GlobalCoreContext;

/// <summary>
/// Saves the current context and makes the passed one current, and then
/// restores the current context when destroyed
/// </summary>
class CurrentContextPusher
{
  // Copy construction not permitted
  CurrentContextPusher(const CurrentContextPusher& rhs);

public:
  /// <summary>
  /// Saves the current context, to be restored on destruction
  /// </summary>
  CurrentContextPusher(void);

  CurrentContextPusher(CoreContext& context);
  CurrentContextPusher(const std::shared_ptr<CoreContext>& pContext);
  CurrentContextPusher(const std::shared_ptr<GlobalCoreContext>& pContext);
  CurrentContextPusher(CoreContext* pContext);
  CurrentContextPusher(CurrentContextPusher&& rhs);
  ~CurrentContextPusher(void);

  /// <summary>
  /// Provides the caller with a way to set the prior context current prior to the destruction of this instance
  /// </summary>
  /// <returns>A reference to the context that was previously current</returns>
  /// <remarks>
  /// This method is idempotent
  /// </remarks>
  std::shared_ptr<CoreContext> Pop(void);

private:
  // True if Pop has already been invoked
  bool pop_invoked = false;

  // Pointer to the prior context--may potentially be nullptr
  std::shared_ptr<CoreContext> m_prior;
};

