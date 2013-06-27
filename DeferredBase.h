// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _DEFERRED_BASE_H
#define _DEFERRED_BASE_H
#include <functional>
#include <vector>
#include FUNCTIONAL_HEADER
#include SHARED_PTR_HEADER

class CoreContext;
class AutowirableSlot;

// Deferred autowiring base class
class DeferredBase {
public:
  DeferredBase(CoreContext* pThis, std::weak_ptr<AutowirableSlot> tracker);
  virtual ~DeferredBase(void);

protected:
  CoreContext* pThis;

  // Functions that want to be called when we successfully bind:
  std::vector< std::function<void()> > m_postBind;

  // Store a weak reference to the slot's tracker so we can be informed
  // if it goes away before we have a chance to autowire it
  std::weak_ptr<AutowirableSlot> tracker;

public:
  void AddPostBindingListener(const std::function<void()>& listener) {
    m_postBind.push_back(listener);
  }

  /// <summary>
  /// Returns true if this deferred base has become expired
  /// </summary>
  /// <remarks>
  /// A deferred base may become expired if its bound slot could not be autowired before
  /// it was destroyed.  At that point, the deferred base should be dropped as expired.
  /// </remarks>
  bool IsExpired(void) const {return tracker.expired();}

  virtual bool operator()() = 0;
};

#endif
