// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _CURRENT_CONTEXT_PUSHER_H
#define _CURRENT_CONTEXT_PUSHER_H
#include SHARED_PTR_HEADER

class CoreContext;

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
  CurrentContextPusher(CoreContext* pContext);
  CurrentContextPusher(CurrentContextPusher&& rhs);
  ~CurrentContextPusher(void);

  /// <summary>
  /// Provides the caller with a way to set the prior context current prior to the destruction of this instance
  /// </summary>
  /// <remarks>
  /// This method is idempotent
  /// </remarks>
  void Pop(void);

private:
  std::shared_ptr<CoreContext> m_prior;

public:
  CoreContext* operator->(void) {return m_prior.get();}
};

#endif
