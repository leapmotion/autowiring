// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _CONTEXT_CREATOR_H
#define _CONTEXT_CREATOR_H
#include "ContextCreatorBase.h"
#include "CoreContext.h"
#include "DeferredCreationNotice.h"
#include STL_UNORDERED_MAP
#include <boost/thread/mutex.hpp>

/// <summary>
/// Implements a foundation class that allows named context registration and augmentation
/// </summary>
/// <param name="contextName">A unique name for this context type</param>
/// <param name="Key">A key type used to identify this context</param>
/// <remarks>
/// This class helps manage the creation of contexts with global names.  When the new child context
/// is created, a notification is broadcast throughout the entire current context to any registered
/// Bolt.
///
/// None of these methods are synchronized.
/// </remarks>
template<const char* contextName, class Key>
class ContextCreator:
  public ContextCreatorBase
{
public:
  ContextCreator(void) {
    m_context = CoreContext::CurrentContext();
  }

protected:
  // Lock:
  boost::mutex m_contextLock;

  // Collection of mapped contexts:
  typedef std::unordered_map<Key, std::shared_ptr<CoreContext>> t_mpType;
  t_mpType m_mp;

  // Local context pointer:
  std::weak_ptr<CoreContext> m_context;

public:
  /// <summary>
  /// Performs a scan of all contexts and evicts those contexts which have terminated
  /// </summary>
  /// <remarks>
  /// Consider eliminating this method and altering the internal map to use weak pointers.
  /// </remarks>
  void EvictStale(void) {
    for(auto q = m_mp.begin(); q != m_mp.end(); )
      if(q->second.unique())
        m_mp.erase(q++);
      else
        q++;
  }

  /// <summary>
  /// Attempts to find a context with the specified key
  /// </summary>
  std::shared_ptr<CoreContext> FindContext(const Key& key) {
    boost::lock_guard<boost::mutex> lk(m_contextLock);
    typename t_mpType::iterator q = m_mp.find(key);
    if(q == m_mp.end())
      return std::shared_ptr<CoreContext>();
    return q->second;
  }

  /// <summary>
  /// Creates a context with the specified key, dependent upon the current context
  /// </summary>
  /// <returns>A deferred creation notice which, when destroyed, will cause clients to be notified of context creation</returns>
  /// <remarks>
  ///
  /// </remarks>
  std::shared_ptr<DeferredCreationNotice> CreateContext(const Key& key) {
    boost::lock_guard<boost::mutex> lk(m_contextLock);

    // Try to find a context already existing with the given key:
    std::shared_ptr<CoreContext>& child = m_mp[key];
    std::shared_ptr<DeferredCreationNotice> retVal;
    if(child)
      retVal.reset(new DeferredCreationNotice(nullptr, child));
    else {
      // Attempt to lock the context.  Could already be destroyed by this point.
      std::shared_ptr<CoreContext> context = m_context.lock();
      if(context) {
        // Create:
        child = context->Create();
        retVal.reset(new DeferredCreationNotice(contextName, child));
      }
    }
    return retVal;
  }

  /// <sumamry>
  /// Removes all contexts from this creator, and optionally waits for them to quit
  /// </summary>
  /// <param name="wait">True if this call should not return until all child contexts quit</param>
  /// <remarks>
  /// The blocking version of the Clear method works by copying the current map into a temporary
  /// container, and then waiting on those elements to terminate.  This behavior allows other users
  /// of this container to add elements even as it's being cleared.
  ///
  /// Regardless of this detail, consumers should be aware that no guarantees are made about how
  /// long this container will remain empty once the function returns in an asynchronous context.
  /// The contaner could, in fact, have elements in it at the time control is returned to the caller.
  /// </remarks>
  void Clear(bool wait) {
    if(!wait) {
      // Trivial signal-clear-return:
      boost::lock_guard<boost::mutex> lk(m_contextLock);
      for(auto q = m_mp.begin(); q != m_mp.end(); q++)
        q->second->SignalShutdown();
      m_mp.clear();
      return;
    }

    t_mpType mp;

    // Copy out and clear:
    (boost::lock_guard<boost::mutex>)m_contextLock,
    mp = m_mp,
    m_mp.clear();

    // Signal everyone first, then wait in a second pass:
    for(auto q = mp.begin(); q != mp.end(); q++)
      q->second->Wait();
  }


  /// <summary>
  /// Removes the specified context by its key
  /// </summary>
  void RemoveContext(const Key& key) {
    boost::lock_guard<boost::mutex> lk(m_contextLock);
    m_mp.erase(key);
  }

  /// <summary>
  /// Removes the specified context by its iterator
  /// </summary>
  void RemoveContext(typename t_mpType::iterator q) {
    boost::lock_guard<boost::mutex> lk(m_contextLock);
    m_mp.erase(q);
  }
};

#endif
