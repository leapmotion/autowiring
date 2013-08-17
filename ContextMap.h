// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _CONTEXT_MAP_H
#define _CONTEXT_MAP_H
#include "CoreContext.h"
#include <boost/thread/mutex.hpp>
#include STL_UNORDERED_MAP

extern std::shared_ptr<CoreContext> NewContextThunk(void);

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
  ContextMap(void):
    m_tracker(new boost::mutex),
    m_lk(*m_tracker)
  {}

  ~ContextMap(void) {
    // Teardown pathway assurance:
    (boost::lock_guard<boost::mutex>)*m_tracker,
    m_tracker.reset();
  }

private:
  // Tracker lock, used to protect against accidental destructor-contending access while still allowing
  // the parent ContextMap structure to be stack-allocated
  std::shared_ptr<boost::mutex> m_tracker;

  typedef std::unordered_map<Key, std::weak_ptr<CoreContext>> t_mpType;
  boost::mutex& m_lk;
  t_mpType m_contexts;

public:
  // Accessor methods:
  size_t size(void) const {return m_contexts.size();}

  template<class Fn>
  void Enumerate(Fn&& fn) {
    boost::lock_guard<boost::mutex> lk(m_lk);
    for(auto q = m_contexts.begin(); q != m_contexts.end(); q++) {
      auto ctxt = q->second.lock();
      fn(q->first, ctxt);
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
  void Add(const Key& key, std::shared_ptr<CoreContext>& context) {
    boost::lock_guard<boost::mutex> lk(m_lk);
    auto& rhs = m_contexts[key];
    if(!rhs.expired())
      throw_rethrowable std::runtime_error("Specified key is already associated with another context");

    rhs = context;

    std::weak_ptr<boost::mutex> tracker(m_tracker);
    context->AddTeardownListener([this, key, tracker] {
      // Prevent the map from being deleted while we process this teardown notice:
      auto locked = tracker.lock();
      if(!locked)
        // Context survived the map
        return;

      boost::lock_guard<boost::mutex> lk(*locked);

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
    });
  }

  /// <summary>
  /// Attempts to find a context by the specified key
  /// </summary>
  std::shared_ptr<CoreContext> Find(const Key& key) {
    boost::lock_guard<boost::mutex> lk(m_lk);
    auto q = m_contexts.find(key);

    return
      q == m_contexts.end() ?
      std::shared_ptr<CoreContext>() :
      q->second.lock();
  }
};

#endif
