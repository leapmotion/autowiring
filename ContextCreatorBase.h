// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _CONTEXT_CREATOR_BASE_H
#define _CONTEXT_CREATOR_BASE_H
#include SHARED_PTR_HEADER
#include <boost/thread/mutex.hpp>

class CoreContext;

class ContextCreatorBase {
public:
  ContextCreatorBase(void);
  virtual ~ContextCreatorBase(void);

protected:
  // Lock:
  boost::mutex m_contextLock;

  // Local context pointer:
  std::weak_ptr<CoreContext> m_context;

  /// <summary>
  /// Utility method for clearing an arbitrary collection:
  /// </summary>
  /// <remarks>
  /// This method is synchronized on the contextLock and this will generally make it thread-safe
  /// </remarks>
  template<class Ctr, class Fx>
  void Clear(Ctr& ctr) {
  }
};

#endif
