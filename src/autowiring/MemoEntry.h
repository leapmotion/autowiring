// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"
#include "once.h"

class CoreContext;

namespace autowiring {

struct CoreObjectDescriptor;
class DeferrableAutowiring;

/// \internal
/// <summary>
/// Represents a single entry, together with a signal which would be fired on the satisfaction of this entry
/// </summary>
struct MemoEntry {
  MemoEntry(void);
  MemoEntry(const MemoEntry& rhs) = delete;

  // A signal which would be fired on the satisfaction of this entry
  autowiring::once_signal<CoreContext> onSatisfied;

  // A back reference to the concrete type from which this memo was generated.  This field may be null
  // if there is no corresponding concrete type.
  const CoreObjectDescriptor* pObjTraits = nullptr;

  // Once this memo entry is satisfied, this will contain the AnySharedPointer instance that performs
  // the satisfaction
  AnySharedPointer m_value;

  // A flag to determine if this memo entry was set from the current context.
  bool m_local = true;
};

}
