// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <typeinfo>

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
  BoltBase(void);
  virtual ~BoltBase(void);

  /// <summary>
  /// Returns a null-termianted list of one (or more) sigil types that this bolt cares about
  /// </summary>
  virtual const t_TypeInfoVector GetContextSigils(void) const = 0;

  /// <summary>
  /// A notification broadcast when a context of the desired name has been created
  /// </summary>
  /// <remarks>
  /// The current context at the time of the call is guaranteed to be the newly created context.
  /// </remarks>
  virtual void ContextCreated(void) = 0;
};
