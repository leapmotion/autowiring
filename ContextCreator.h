// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "ContextCreatorBase.h"
#include "CoreContext.h"
#include STL_UNORDERED_MAP

/// <summary>
/// Implements a foundation class that allows named context registration and augmentation
/// </summary>
/// <param name="Sigil">The sigil type that will be used for created contexts</param>
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
template<class Sigil, class Key = void>
class ContextCreator:
  public ContextCreatorBase
{
protected:
  // Collection of mapped contexts:
  typedef std::unordered_map<Key, std::weak_ptr<CoreContext>> t_mpType;
  t_mpType m_contexts;

  // The type that will be used with destruction notification callbacks
  typedef Key t_callbackHandle;

public:
  // Accessor methods:
  size_t GetSize(void) const {return m_contexts.size();}

  /// <summary>
  /// Enumeration routine, similar to the ContextMap enumerator
  /// </summary>
  template<class Fn>
  void Enumerate(Fn&& fn) {
    boost::lock_guard<boost::mutex> lk(m_contextLock);
    for(auto q = m_contexts.begin(); q != m_contexts.end(); q++) {
      auto ctxt = q->second.lock();
      if(ctxt)
        if(!fn(q->first, ctxt))
          return;
    }
  }

  /// <summary>
  /// Reads out all contained contexts into the specified container type
  /// </summary>
  template<class Container>
  Container Enumerate(void) {
    Container container;
    boost::lock_guard<boost::mutex> lk(m_contextLock);
    for(auto q = m_contexts.begin(); q != m_contexts.end(); q++) {
      auto ctxt = q->second.lock();
      if(ctxt)
        container.insert(container.end(), ctxt);
    }
    return container;
  }

  /// <summary>
  /// Attempts to find a context with the specified key
  /// </summary>
  std::shared_ptr<CoreContext> FindContext(const Key& key) {
    boost::lock_guard<boost::mutex> lk(m_contextLock);
    typename t_mpType::iterator q = m_contexts.find(key);
    if(q == m_contexts.end())
      return std::shared_ptr<CoreContext>();
    return q->second.lock();
  }

  /// <summary>
  /// Creates a context with the specified key, dependent upon the current context
  /// </summary>
  /// <returns>A pair in which the first element is the created context and the second is a bool set if creation took place</returns>
  std::pair<std::shared_ptr<CoreContext>, bool> CreateContext(const Key& key) {
    boost::lock_guard<boost::mutex> lk(m_contextLock);

    // Obtain and lock a weak pointer, if possible:
    auto& childWeak = m_contexts[key];
    auto retVal = childWeak.lock();

    // Try to find a context already existing with the given key:
    if(retVal)
      return std::make_pair(retVal, false);

    // Attempt to lock the enclosing context.  Could already be destroyed by this point.
    auto context = m_context.lock();
    if(!context)
      // Can't lock the context, empty pointer returned
      throw std::runtime_error("Attempted to create a child context of a parent context which was shutting down");

    // Create, and insert into our map:
    retVal = context->Create<Sigil>();
    childWeak = retVal;

    // Add a teardown listener so we know when to evict from our map:
    auto pContext = retVal.get();
    retVal->AddTeardownListener([this, key, pContext] () {
      this->NotifyContextDestroyed(key, pContext);
    });
    return std::make_pair(retVal, true);
  }

  /// <summary>
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
    ContextCreatorBase::Clear(wait, m_contexts, [] (typename t_mpType::iterator q) {return q->second.lock();});
  }

  /// <summary>
  /// Removes the specified context by its key
  /// </summary>
  /// <returns>The removed context, if one existed, otherwise nullptr</returns>
  std::shared_ptr<CoreContext> RemoveContext(const Key& key) {
    boost::lock_guard<boost::mutex> lk(m_contextLock);
    auto q = m_contexts.find(key);
    if(q == m_contexts.end())
      return std::shared_ptr<CoreContext>();

    auto retVal = q->second.lock();
    m_contexts.erase(q);
    return retVal;
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
    m_contexts.erase(q);
  }

  /// <summary>
  /// Notifies this context creator that the specified context is being destroyed
  /// </summary>
  /// <param name="key">The key that was originally used to create the context</param>
  /// <remarks>
  /// The default behavior of this method is to evict the key from the internal map.  Consumers who desire
  /// to change the default behavior of this map should pass control to the base class.
  ///
  /// Consumers are urged to exercise caution when attempting to dereference pContext, as pContext will be
  /// in a teardown pathway when this method is called.
  /// </remarks>
  virtual void NotifyContextDestroyed(t_callbackHandle key, CoreContext* pContext) {
    (boost::lock_guard<boost::mutex>)m_contextLock,
    m_contexts.erase(key);
  }
};

/// <summary>
/// Specialization for consumers who do not wish (or have any need) to key their contexts
/// </summary>
template<class Sigil>
class ContextCreator<Sigil, void>:
  public ContextCreatorBase
{
protected:
  // List of mapped contexts:
  typedef std::list<std::weak_ptr<CoreContext>> t_contextList;
  t_contextList m_contextList;

  // The type that will be used with destruction notification callbacks
  typedef t_contextList::iterator t_callbackHandle;

  unsigned char m_clearSentinal;

public:
  ContextCreator() : m_clearSentinal(0) {}

  // Accessor methods:
  size_t GetSize(void) const {return m_contextList.size();}

  /// <summary>
  /// Unconditionally creates a context
  /// </summary>
  /// <returns>A shared pointer to the created context</returns>
  std::shared_ptr<CoreContext> CreateContext(void) {
    // Attempt to lock the context.  Could already be destroyed by this point.
    std::shared_ptr<CoreContext> context = m_context.lock();
    if(!context)
      return std::shared_ptr<CoreContext>();

    // Create:
    auto child = context->Create<Sigil>();
    t_callbackHandle q;
    unsigned short sentinal;
    // Insert into our list
    {
      boost::lock_guard<boost::mutex> lock(m_contextLock);
      m_contextList.push_front(child);
      q = m_contextList.begin();
      sentinal = m_clearSentinal;
    }

    // Add a teardown listener so we can update the list:
    auto pContext = child.get();
    child->AddTeardownListener([this, q, pContext, sentinal] () {
      //invalidate the iterator if we've detected that it has already been removed
      //by a call to Clear
      if( (boost::lock_guard<boost::mutex>)m_contextLock, sentinal != m_clearSentinal ) {
        this->NotifyContextDestroyed(m_contextList.end(), pContext);
      }
      else {
        this->NotifyContextDestroyed(q, pContext);
        (boost::lock_guard<boost::mutex>)m_contextLock,
          m_contextList.erase(q);
      }
    });
    return child;
  }

  /// <summary>
  /// Enumeration routine, similar to the ContextMap enumerator
  /// </summary>
  template<class Fn>
  void Enumerate(Fn&& fn) {
    boost::lock_guard<boost::mutex> lk(m_contextLock);
    for(auto q = m_contextList.begin(); q != m_contextList.end(); q++) {
      auto ctxt = q->lock();
      if(ctxt)
      if(!fn(ctxt))
        return;
    }
  }

  /// <summary>
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
  /// The container could, in fact, have elements in it at the time control is returned to the caller.
  /// </remarks>
  void Clear(bool wait) {
    (boost::lock_guard<boost::mutex>)m_contextLock,
      m_clearSentinal++;
    ContextCreatorBase::Clear(wait, m_contextList, [] (typename t_contextList::iterator q) { return q->lock();});
  }

  /// <summary>
  /// Notifies this context creator that the specified context is being destroyed
  /// </summary>
  /// <param name="q">An iterator referring to the context being destroyed</param>
  /// <remarks>
  /// The default behavior of this method is to evict the key from the internal map.  Consumers who desire
  /// to change the default behavior of this map should pass control to the base class.
  ///
  /// Consumers are urged to exercise caution when attempting to dereference pContext, as pContext will be
  /// in a teardown pathway when this method is called.
  /// </remarks>
  virtual void NotifyContextDestroyed(t_callbackHandle q, CoreContext* pContext) { }
};
