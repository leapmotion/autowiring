// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _CONTEXT_MAP_H
#define _CONTEXT_MAP_H
#include <boost/thread/mutex.hpp>
#include <map>
#include <string>

class CoreContext;

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
private:
  typedef std::map<Key, std::weak_ptr<CoreContext> > t_mpType;
  boost::mutex m_lk;
  t_mpType m_contexts;

  void ProximityCheck(typename ContextMap<Key>::t_mpType::iterator q) {
    if(
      ++q != m_contexts.end() &&
      q->second.expired()
    )
      m_contexts.erase(q);
  }

public:
  // Accessor methods:
  size_t size(void) const {return m_contexts.size();}

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
    typename t_mpType::iterator q = m_contexts.lower_bound(key);
    if(
      q != m_contexts.end() &&
      q->first == key
    )
      throw_rethrowable std::runtime_error("Specified key is already associated with another context");
    q = m_contexts.insert(q, typename t_mpType::value_type(key, context));
    ProximityCheck(q);
  }

  /// <summary>
  /// Attempts to find a context by the specified key
  /// </summary>
  std::shared_ptr<CoreContext> Find(const Key& key) {
    std::shared_ptr<CoreContext> retVal;

    boost::lock_guard<boost::mutex> lk(m_lk);
    typename t_mpType::iterator q = m_contexts.lower_bound(key);
    if(q != m_contexts.end()) {
      if(q->first == key)
        retVal = q->second.lock();
      ProximityCheck(q);
    }

    return retVal;
  }

  /// <summary>
  /// Identifies and locks the context specified by the key, if one exists, or creates it if one does not
  /// </summary>
  std::shared_ptr<CoreContext> Create(const Key& key) {
    std::shared_ptr<CoreContext> retVal;

    // Lock and lookup:
    boost::lock_guard<boost::mutex> lk(m_lk);
    typename t_mpType::iterator q = m_contexts.lower_bound(key);
    if(
      q == m_contexts.end() ||
      q->first != key ||
      !(retVal = q->second.lock())
    ) {
      // Not present, create and return:
      retVal = NewContextThunk();
      q = m_contexts.insert(q, t_mpType::value_type(key, retVal));
    }

    // Cleanup check:
    ProximityCheck(q);

    // Done
    return retVal;
  }
};

#endif
