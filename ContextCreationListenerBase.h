#ifndef _CONTEXT_CREATION_LISTENER_BASE_H
#define _CONTEXT_CREATION_LISTENER_BASE_H
#include "EventReceiver.h"
#include <memory>

class CoreContext;

/// <summary>
/// </summary>
/// <param name="contextName">The name of the context whose creation we want to listen for</param>
/// <remarks>
/// </remarks>
class ContextCreationListenerBase:
  public EventReceiver
{
public:
  /// <summary>
  /// A notification broadcast when a context of the desired name has been created
  /// </summary>
  /// <param name="context">A reference to the context that was just created</param>
  virtual void ContextCreated(const std::shared_ptr<CoreContext>& context) = 0;
};

#endif