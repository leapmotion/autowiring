// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_id.h"

class CoreContext;

/// <summary>
/// </summary>
/// <remarks>
/// </remarks>
class BoltBase
{
public:
  BoltBase(void);
  virtual ~BoltBase(void);

  /// <summary>
  /// Returns a null-terminated list of one (or more) sigil types that this bolt cares about
  /// </summary>
  virtual const auto_id* GetContextSigils(void) const = 0;

  /// <returns>
  /// True if the specified sigil type matches the types interesting to this bolt
  /// </returns>
  virtual bool Matches(auto_id id) const = 0;

  /// <summary>
  /// A notification broadcast when a context of the desired name has been created
  /// </summary>
  /// <remarks>
  /// The current context at the time of the call is guaranteed to be the newly created context.
  ///
  /// This method may potentially be called at an arbitrary time _after_ the context is created.
  /// The created context may not yet be started, or it could be initiated, or it could be in a
  /// teardown state, or any other state that is valid for a context.
  /// </remarks>
  virtual void ContextCreated(void) = 0;
};
