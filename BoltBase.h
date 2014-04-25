// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _CONTEXT_CREATION_LISTENER_BASE_H
#define _CONTEXT_CREATION_LISTENER_BASE_H
#include <memory>
#include <vector>
#include TYPE_INDEX_HEADER
class CoreContext;

typedef const std::type_info*const* t_TypeInfoVector;

/// <summary>
/// </summary>
/// <param name="contextName">The name of the context whose creation we want to listen for</param>
/// <remarks>
/// </remarks>
class BoltBase
{
public:
  virtual ~BoltBase(void) {}

  /// <summary>
  /// Returns a null-termianted list of one (or more) sigil types that this bolt cares about
  /// </summary>
  virtual const t_TypeInfoVector GetContextSigils(void) = 0;

  /// <summary>
  /// A notification broadcast when a context of the desired name has been created
  /// </summary>
  /// <remarks>
  /// The current context at the time of the call is guaranteed to be the newly created context.
  /// </remarks>
  virtual void ContextCreated(void) = 0;
};

#endif
