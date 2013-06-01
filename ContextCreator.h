// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _CONTEXT_CREATOR_H
#define _CONTEXT_CREATOR_H
#include "ContextCreatorBase.h"
#include "CoreContext.h"
#include "DeferredCreationNotice.h"
#if defined(__APPLE__) && !defined(_LIBCPP_VERSION)
#include <tr1/unordered_map>
#else
#include <unordered_map>
#endif

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

  /// <summary>
  /// Removes the specified context by its key
  /// </summary>
  void RemoveContext(const Key& key) {
    m_mp.erase(key);
  }

  /// <summary>
  /// Removes the specified context by its iterator
  /// </summary>
  void RemoveContext(typename t_mpType::iterator q) {
    m_mp.erase(q);
  }
};

#endif
