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

  CurrentContextPusher(std::shared_ptr<CoreContext> pContext);
  CurrentContextPusher(std::shared_ptr<GlobalCoreContext> pContext);
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
  std::shared_ptr<CoreContext> m_prior;
};

