// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _AUTOWIRABLE_SLOT_H
#define _AUTOWIRABLE_SLOT_H
#include "AutoFactory.h"
#include FUNCTIONAL_HEADER
#include SHARED_PTR_HEADER
#include <functional>
#include <memory>

class CoreContext;

// Utility routine, for users who need a function that does nothing
template<class T>
void NullOp(T) {}

class AutowirableSlot {
private:
  // Copy construction of an autowired slot is generally unsafe and not allowed
  AutowirableSlot(const AutowirableSlot& rhs);

public:
  AutowirableSlot(std::weak_ptr<CoreContext> context);

  virtual ~AutowirableSlot(void) {
  }

  /// <summary>
  /// This is a destroy self-reference.  There should only ever be one shared pointer
  /// to this object, and it's located here.  Everything else should be weak pointers
  /// whose purpose is to be notified when this tracker goes away.
  /// </summary>
  std::shared_ptr<AutowirableSlot> m_tracker;

  /// <summary>
  /// This is the context that was available at the time the autowiring was performed.
  /// </summary>
  /// <remarks>
  /// A weak reference is held in hroder to ensure proper teardown, otherwise the
  /// first created member of the context would generate a cyclic reference between
  /// this pointer and the context membership set.
  /// </remarks>
  std::weak_ptr<CoreContext> m_context;

  /// <summary>
  /// Convenience method, functionally identical to CoreContext::NotifyWhenAutowired
  /// </summary>
  void NotifyWhenAutowired(const std::function<void()>& listener);

  /// <summary>
  /// Utility routine to lock the context, or throw an exception if something goes wrong
  /// </summary>
  std::shared_ptr<CoreContext> LockContext(void);

  virtual bool IsAutowired(void) const = 0;
};

#endif
