// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _DEFERRED_BASE_H
#define _DEFERRED_BASE_H
#include <functional>
#include <memory>
#include <vector>

class Autowirer;
class AutowirableSlot;

// Deferred autowiring base class
class DeferredBase {
public:
  DeferredBase(Autowirer* pThis, std::weak_ptr<AutowirableSlot> tracker);
  virtual ~DeferredBase(void);

protected:
  Autowirer* pThis;

  // Functions that want to be called when we successfully bind:
  std::vector< std::function<void()> > m_postBind;
    
  // Store a weak reference to the slot's tracker so we can be informed
  // if it goes away before we have a chance to autowire it
  std::weak_ptr<AutowirableSlot> tracker;

public:
  void AddPostBindingListener(const std::function<void()>& listener) {
    m_postBind.push_back(listener);
  }

  virtual bool operator()() = 0;
};

#endif
