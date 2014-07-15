// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once
#include MEMORY_HEADER
#include MUTEX_HEADER

class CoreContext;

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
    if(!wait) {
      // Trivial signal-clear-return:
      std::lock_guard<std::mutex> lk(m_contextLock);
      for(auto q = ctr.begin(); q != ctr.end(); q++) {
        auto locked = locker(q);
        if(locked)
          locked->SignalShutdown();
      }
      ctr.clear();
      return;
    }

    Ctr ctrCopy;

    // Copy out and clear:
    {
      std::lock_guard<std::mutex> lk(m_contextLock);
      for(auto q = ctr.begin(); q != ctr.end(); q++) {
        auto locked = locker(q);
        if(locked)
          locked->SignalShutdown();
      }
      ctrCopy = ctr;
      ctr.clear();
    }

    // Signal everyone first, then wait in a second pass:
    for(auto q = ctrCopy.begin(); q != ctrCopy.end(); q++) {
      auto locked = locker(q);
      if(locked)
        locked->Wait();
    }
  }
};
