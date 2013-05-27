#ifndef _CONTEXT_CREATION_LISTENER_H
#define _CONTEXT_CREATION_LISTENER_H
#include "ContextCreationListenerBase.h"

/// <summary>
/// </summary>
/// <param name="contextName">The name of the context whose creation we want to listen for</param>
/// <remarks>
/// </remarks>
template<const char* contextName>
class ContextCreationListener:
  public ContextCreationListenerBase
{
};

#endif