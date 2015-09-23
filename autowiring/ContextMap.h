// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "autowiring_error.h"
#include "CoreContext.h"
#include <map>

namespace autowiring {
  struct tracker
  {
    // Lock proper
    std::mutex lock;

    // True when the enclosing context map has been destroyed
    bool destroyed = false;
  };
}

/// <summary>
/// A context map allows the management of semitransient contexts
/// </summary>
/// <remarks>
/// Context members are allowed to create and manage their own subcontexts at will.  In order
/// to ensure that the map remains accurate, however, it's necessary to evict any contexts
/// that exit, and it's also necessary to avoid allowing natural cleanup with a context's
/// reference count goes to zero.
///
/// The context map solves this by maintaining weak pointers to the contexts it knows about,
/// and registers listeners that wait for context termination and clean up the map accordingly.
///
/// All of the context map members are thread-safe.
/// </remarks>
template<class Key>
class ContextMap
{
public:
  ContextMap(void) = default;

  ~ContextMap(void) {
    // Teardown pathway assurance:
    std::lock_guard<std::mutex> lk(m_tracker->lock);
    m_tracker->destroyed = true;
  }

private:
  // Tracker lock, used to protect against accidental destructor-contending access while still allowing
  // the parent ContextMap structure to be stack-allocated
  const std::shared_ptr<autowiring::tracker> m_tracker = std::make_shared<autowiring::tracker>();
  typedef std::map<Key, std::weak_ptr<CoreContext>> t_mpType;
  t_mpType m_contexts;

public:
  // Accessor methods:
  size_t size(void) const {return m_contexts.size();}

  /// <summary>
  /// Removes all elements from the map
  /// </summary>
  void clear(void) {
    std::lock_guard<std::mutex> lk(m_tracker->lock);
    m_contexts.clear();
  }

  class iterator {
  public:
    iterator(const ContextMap& parent) :
      parent(parent)
    {
      std::lock_guard<std::mutex> lk(parent.m_tracker->lock);

      // Advance to the first iterator we can actually lock down:
      iter = parent.m_contexts.begin();
      while (
        iter != parent.m_contexts.end() &&
        !(ctxt = iter->second.lock())
      )
        // Failure, next entry
        iter++;
    }
      
    iterator(const ContextMap& parent, typename t_mpType::const_iterator iter, std::shared_ptr<CoreContext> ctxt) :
      parent(parent),
      iter(iter),
      ctxt(ctxt)
    {}

  private:
    const ContextMap& parent;
    typename t_mpType::const_iterator iter;
    std::shared_ptr<CoreContext> ctxt;

  public:
    const iterator& operator++(void) {
      // We need to ensure our local shared pointer doesn't go away until after we have
      // advanced to the next entry
      std::shared_ptr<CoreContext> deferred = std::move(ctxt);

      // Loop until we get another stable entry:
      std::lock_guard<std::mutex> lk(parent.m_tracker->lock);
      do iter++;
      while (iter != parent.m_contexts.end() && !(ctxt = iter->second.lock()));

      // We can safely unlock because the current entry won't be evicted automatically as long as
      // the shared pointer reference is held down.
      return *this;
    }

    const std::shared_ptr<CoreContext>& operator*(void) const { return ctxt; }
    const CoreContext& operator->(void) const { return *ctxt; }
    bool operator==(const iterator& rhs) const { return &parent == &rhs.parent && iter == rhs.iter; }
    bool operator!=(const iterator& rhs) const { return !(*this == rhs); }
    explicit operator bool(void) const {
      return ctxt;
    }
  };

  typedef iterator const_iterator;

  iterator begin(void) const { return iterator(*this); }
  iterator end(void) const { return iterator(*this, m_contexts.end(), nullptr); }

  template<class Fn>
  void Enumerate(Fn&& fn) {
    std::lock_guard<std::mutex> lk(m_tracker->lock);
    for(const auto& entry : m_contexts) {
      auto ctxt = entry.second.lock();
      if(ctxt && !fn(entry.first, ctxt))
        return;
    }
  }

  /// <summary>
  /// Adds a new context to the map
  /// </summary>
  /// <remarks>
  /// The context will be tracked until its reference count hits zero.  This method does not
  /// alter the reference count of the passed context.
  ///
  /// An exception will be thrown if the passed key is already associated with a context
  /// </remarks>
  void Add(const Key& key, const std::shared_ptr<CoreContext>& context) {
    std::lock_guard<std::mutex> lk(m_tracker->lock);
    auto& rhs = m_contexts[key];
    if(!rhs.expired())
      throw autowiring_error("Specified key is already associated with another context");

    rhs = context;

    std::weak_ptr<autowiring::tracker> tracker(m_tracker);
    context->onTeardown += [this, key, tracker] (const CoreContext&) {
      // Prevent the map from being deleted while we process this teardown notice:
      auto locked = tracker.lock();
      if(!locked)
        // Context survived the map
        return;

      std::lock_guard<std::mutex> lk(locked->lock);
      if(locked->destroyed)
        // Map passed through teardown pathway while we were trying to lock it
        return;

      // We only remove the key if it's expired.  Under normal circumstances, the key will
      // be expired by the time we get here, but there is a small chance that the same key
      // will be introduced at the exact time that the context is tearing down, which could
      // cause the slot for that key to be reclaimed earlier than expected.
      auto q = m_contexts.find(key);

      // There is a very unlikely race which could cause the key to not be found.  It involves
      // a second context being created and destroyed on the same key as some original context.
      // This causes one of the two context's teardown handlers to attempt to evict the same
      // key at the same time, and one of them will of course fail.
      if(q == m_contexts.end())
        return;

      // Try to lock--potentially, this key has been reclaimed by a different context, and in
      // that case, the new context will gain the responsibility of tearing down this key when
      // the time comes.
      auto sp = q->second.lock();
      if(!sp)
        this->m_contexts.erase(key);
    };
  }

  /// <summary>
  /// Attempts to find a context by the specified key
  /// </summary>
  std::shared_ptr<CoreContext> Find(const Key& key) const {
    std::lock_guard<std::mutex> lk(m_tracker->lock);
    auto q = m_contexts.find(key);

    return
      q == m_contexts.end() ?
      nullptr :
      q->second.lock();
  }

  /// <summary>
  /// Identical to Find
  /// </summary>
  std::shared_ptr<CoreContext> operator[](const Key& key) const {
    return Find(key);
  }
};
