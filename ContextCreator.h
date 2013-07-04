// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _CONTEXT_CREATOR_H
#define _CONTEXT_CREATOR_H
#include "ContextCreatorBase.h"
#include "CoreContext.h"
#include "DeferredCreationNotice.h"
#include STL_UNORDERED_MAP

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
/// If the key type is not specified, contexts will be unconditionally created.  In this case, a
/// context can only be removed by its iterator.
///
/// All static member functions are thread-safe, other members are not thread-safe.
/// </remarks>
template<const char* contextName, class Key = void>
class ContextCreator:
  public ContextCreatorBase
{
protected:
  // Collection of mapped contexts:
  typedef std::unordered_map<Key, std::weak_ptr<CoreContext>> t_mpType;
  t_mpType m_mp;

public:
  // Accessor methods:
  size_t GetSize(void) const {return m_mp.size();}

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
  std::shared_ptr<DeferredCreationNotice> CreateContext(const Key& key) {
    boost::lock_guard<boost::mutex> lk(m_contextLock);

    // Obtain and lock a weak pointer, if possible:
    auto& childWeak = m_mp[key];
    auto child = childWeak.lock();

    // Try to find a context already existing with the given key:
    std::shared_ptr<DeferredCreationNotice> retVal;
    if(child)
      retVal.reset(new DeferredCreationNotice(nullptr, child));
    else {
      // Attempt to lock the context.  Could already be destroyed by this point.
      std::shared_ptr<CoreContext> context = m_context.lock();
      if(context) {
        // Create:
        child = context->Create();
        childWeak = child;
        retVal.reset(new DeferredCreationNotice(contextName, child));

        // Add a teardown listener for this child in particular:
        child->AddTeardownListener([this, key] () {
          // Erase the key from our collection:
          (boost::lock_guard<boost::mutex>)m_contextLock,
          m_mp.erase(key);
        });
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
    ContextCreatorBase::Clear(m_mp, [] (t_mpType::iterator q) {return q->second.lock();});
  }

  /// <summary>
  /// Removes the specified context by its key
  /// </summary>
  void RemoveContext(const Key& key) {
    (boost::lock_guard<boost::mutex>)m_contextLock,
    m_mp.erase(key);
  }

  /// <summary>
  /// Removes the specified context by its iterator
  /// </summary>
  /// <remarks>
  /// Removing a context does not do anything to the context itself.  The caller is responsible for ensuring
  /// that the context is shut down (if this is desired) or performing any required synchronization.
  ///
  /// If the caller does _not_ do this, then the context is in a so-called "orphaned state," whereby all
  /// shared pointers to the context are held by threads currently running in the context.
  /// </remarks>
  void RemoveContext(typename t_mpType::iterator q) {
    (boost::lock_guard<boost::mutex>)m_contextLock,
    m_mp.erase(q);
  }

  /// <summary>
  /// Notifies this context creator that the specified context is being destroyed
  /// </summary>
  /// <param name="key">The key that was originally used to create the context</param>
  /// <remarks>
  /// The default behavior of this method is to evict the key from the internal map.  Consumers who desire
  /// to change the default behavior of this map should pass control to the base class.
  /// </remarks>
  virtual void NotifyContextDestroyed(Key key, CoreContext* pContext) {
    (boost::lock_guard<boost::mutex>)m_contextLock,
    m_mp.erase(key);
  }
};

/// <summary>
/// Specialization for consumers who do not wish (or have any need) to key their contexts
/// </summary>
template<const char* contextName>
class ContextCreator<contextName, void>:
  public ContextCreatorBase
{
protected:
  // List of mapped contexts:
  typedef std::list<std::weak_ptr<CoreContext>> t_contextList;
  t_contextList m_contextList;

public:
  // Accessor methods:
  size_t GetSize(void) const {return m_contextList.size();}

  /// <summary>
  /// Unconditionally creates a context
  /// </summary>
  /// <returns>A deferred creation notice which, when destroyed, will cause clients to be notified of context creation</returns>
  std::shared_ptr<DeferredCreationNotice> CreateContext(void) {
    // Attempt to lock the context.  Could already be destroyed by this point.
    std::shared_ptr<CoreContext> context = m_context.lock();
    if(!context)
      return std::shared_ptr<DeferredCreationNotice>();

    // Create:
    auto child = context->Create();
    childWeak = child;
    std::shared_ptr<DeferredCreationNotice> retVal(new DeferredCreationNotice(contextName, child));

    // Insert into our list:
    auto q =
      (boost::lock_guard<boost::mutex>)m_contextLock,
      m_contextList.push_front(child),
      m_contextList.begin();

    // Add a teardown listener so we can update the list:
    child->AddTeardownListener([this, q] () {
      (boost::lock_guard<boost::mutex>)m_contextLock,
      m_contextList.erase(q);
    });
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
    ContextCreatorBase::Clear(m_contextList, [] (t_contextList::iterator q) {return q->lock();});
  }
};

#endif
