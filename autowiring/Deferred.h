// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

class CoreThread;

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

  Deferred(CoreThread* pThread) {}
};
