#ifndef _CONTEXT_CREATION_LISTENER_H
#define _CONTEXT_CREATION_LISTENER_H
#include "BoltBase.h"

/// <summary>
/// </summary>
/// <param name="contextName">The name of the context whose creation we want to listen for</param>
/// <remarks>
/// A bolt is a way to augment the creation of a new context in a declarative, by-name manner.
/// </remarks>
template<const char* contextName>
class Bolt:
  public BoltBase
{
public:
  const char* GetContextName(void) override {
    return contextName;
  }
};

#endif