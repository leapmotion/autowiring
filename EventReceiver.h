// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _EVENT_RECEIVER_H
#define _EVENT_RECEIVER_H
#include <assert.h>

class CoreThread;
class TransientContextMember;

bool CoreThreadCanAccept(CoreThread* pThread);

/// <summary>
/// Marker return type for deferred calls
/// </summary>
/// <remarks>
/// Any deferred event must return this type.  Event interfaces may contain a mix of fired and
/// deferred calls.  If a default implementation for a deferred call is desired, it should return
/// the FatalDeferredInvocation type.  Such an overridden function may potentially be called, and
/// will throw an exception for any deferred listener which does not override the handler.
/// </remarks>
class Deferred {
public:
  Deferred(void) {}

  Deferred(TransientContextMember* pMember) {}

  Deferred(CoreThread* pThread) {}
};

/// <summary>
/// Utility type which asserts when construction is attempted
/// </summary>
/// <remarks>
/// Any consumer who attempts to invoke a deferred 
/// </remarks>
class FatalDeferredInvocation:
  public Deferred
{
public:
  FatalDeferredInvocation(void) {
    assert(false);
  }
};

/// <summary>
/// Used to identify event recipients
/// </summary>
class EventReceiver {
public:
  virtual ~EventReceiver(void) {
  }
};

#endif
