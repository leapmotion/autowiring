// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include MEMORY_HEADER
#include MUTEX_HEADER

class CoreContext;

/// ContextCreatorBase
class ContextCreatorBase {
public:
  ContextCreatorBase(void);
  virtual ~ContextCreatorBase(void);

protected:
  // Lock:
  std::mutex m_contextLock;

  // Local context pointer:
  std::weak_ptr<CoreContext> m_context;

  /// <summary>
  /// Utility method for clearing an arbitrary collection:
  /// </summary>
  /// <param name="ctr">The collection of contexts to be cleared</param>
  /// <param name="iter">An iterator locker, which can obtain a shared pointer from an iterator</param>
  /// <remarks>
  /// This method is synchronized on the contextLock and this will generally make it thread-safe
  /// </remarks>
  template<class Ctr, class Fx>
  void Clear(bool wait, Ctr& ctr, Fx&& locker) {
    // Copy out under lock
    Ctr ctrCopy = (std::lock_guard<std::mutex>(m_contextLock), std::move(ctr));
    
    for (const auto& ctxt : ctrCopy) {
      auto locked = locker(ctxt);
      if(locked)
        locked->SignalShutdown();
    }

    if (!wait)
      return;

    // Signal everyone first, then wait in a second pass:
    for(const auto& ctxt : ctrCopy) {
      auto locked = locker(ctxt);
      if(locked)
        locked->Wait();
    }
  }
};
