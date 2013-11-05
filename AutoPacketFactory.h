#pragma once
#include "Autowired.h"
#include "AutoPacketSubscriber.h"
#include "Decompose.h"
#include "FilterPropertyExtractor.h"
#include "ObjectPool.h"
#include <boost/any.hpp>
#include TYPE_INDEX_HEADER
#include STL_UNORDERED_MAP
#include <vector>

class AutoPacket;
class AutoPacketListener;
class Deferred;
class DispatchQueue;

template<class T, bool is_deferred>
struct CallExtractor {
  typedef void(*t_call)(void*, const AutoPacket&);

  t_call operator()() const {
    typedef decltype(&T::AutoFilter) t_fnType;
    return reinterpret_cast<t_call>(
      &BoundCall<AutoPacket, t_fnType, &T::AutoFilter>::Call
    );
  }
};

template<class T>
struct CallExtractor<T, true> {
  typedef void(*t_call)(void*, const AutoPacket&);

  static void CallDeferred(T* pObj, const AutoPacket& repo) {
    const t_call call =
      reinterpret_cast<t_call>(
        &BoundCall<AutoPacket, decltype(&T::AutoFilter), &T::AutoFilter>::Call
      );
    *pObj += [pObj, &repo, call] {
      call(pObj, repo);
    };
  }

  t_call operator()() const {
    return reinterpret_cast<t_call>(&CallDeferred);
  }
};

/// <summary>
/// A configurable factory class for pipeline packets with a built-in object pool
/// </summary>
/// <remarks>
/// Generally, only one packet factory is required per context.
/// </remarks>
class AutoPacketFactory
{
public:
  struct AdjacencyEntry {
    AdjacencyEntry(const std::type_info& ti) :
      ti(ti)
    {}

    // Reflexive type information for this entry
    const std::type_info& ti;

    // Indexes into the subscriber satisfaction vector.  Each entry in this list
    // represents a single subscriber, and an offset in the m_subscribers vector
    std::vector<size_t> subscribers;
  };

  AutoPacketFactory(void);
  ~AutoPacketFactory(void);

private:
  struct AutoPacketResetter {
    AutoFired<AutoPacketListener> m_apl;

    void operator()(AutoPacket& packet) const;
  };

  /// <summary>
  /// An independently maintained object pool just for packets
  /// </summary>
  /// <remarks>
  /// The object pool defined here is provided mainly to allow detection of
  /// pipeline packet expiration in order to support expiration notification
  /// broadcasts.
  /// </remarks>
  ObjectPool<AutoPacket, AutoPacketResetter> m_packets;

  // Vector of known subscribers--a vector must be used because random access is
  // required due to its use in an adjacency list.
  std::vector<AutoPacketSubscriber> m_subscribers;

  // Map used to associate a decorator type with the adjacency entries for that type.
  typedef std::unordered_map<std::type_index, AdjacencyEntry> t_decMap;
  t_decMap m_decorations;

  // Map used to associate a subscriber type with a monotonic index.  The index is
  // an offset into m_subscribers.
  typedef std::unordered_map<std::type_index, size_t> t_subMap;
  t_subMap m_subMap;

public:
  // Accessor methods:
  const std::vector<AutoPacketSubscriber>& GetSubscriberVector(void) const { return m_subscribers; }

  /// <summary>
  /// Finds the packet subscriber proper corresponding to a particular subscriber type
  /// </summary>
  /// <returns>The subscriber, or nullptr if one cannot be found</returns>
  const AutoPacketSubscriber* FindSubscriber(const std::type_info& info) const {
    size_t subscriberIndex = FindSubscriberIndex(info);
    if(!~subscriberIndex)
      return nullptr;
    return &m_subscribers[subscriberIndex];
  }

  /// <summary>
  /// Finds the monotonic index corresponding to a particular subscriber type
  /// </summary>
  /// <returns>The index, or ~0 if the subscriber does not exist</returns>
  size_t FindSubscriberIndex(const std::type_info& info) const {
    auto q = m_subMap.find(info);
    return q == m_subMap.end() ? ~0 : q->second;
  }

  /// <summary>
  /// Finds the entry in the adjacency list corresponding to a decorator type
  /// </summary>
  /// <returns>The adjacency entry, or nullptr if no such entry exists</returns>
  const AdjacencyEntry* FindDecorator(const std::type_info& info) const {
    auto q = m_decorations.find(info);
    return q == m_decorations.end() ? nullptr : &q->second;
  }

  /// <returns>
  /// True if the specified type is a subscriber registered with this factory
  /// </returns>
  template<class T>
  bool IsSubscriber(void) {
    return IsSubscriber(typeid(T));
  }

  bool IsSubscriber(const std::type_info& ti) {
    return m_subMap.find(ti) != m_subMap.end();
  }

  /// <summary>
  /// Registers the passed subscriber, if it defines a method called AutoFilter
  /// </summary>
  template<class T>
  void AddSubscriber(AutoPacketSubscriber&& rhs) {
    typedef Decompose<decltype(&T::AutoFilter)> t_decomposition;

    // Cannot register a subscriber with zero arguments:
    static_assert(t_decomposition::N, "Cannot register a subscriber whose AutoFilter method is arity zero");

    // Determine whether this subscriber already exists--perhaps, it is formerly disabled
    auto q = m_subMap.find(typeid(T));
    size_t subscriberIndex;
    if(q != m_subMap.end()) {
      AutoPacketSubscriber& entry = m_subscribers[q->second];
      subscriberIndex = q->second;

      if(!entry.empty())
        // Already registered, no need to register a second time
        return;

      // Registered but previously removed, re-initialize
      entry = AutoPacketSubscriber(sub);
    }
    else {
      // Register the subscriber and pre-populate the arity:
      subscriberIndex = m_subscribers.size();
      m_subMap[typeid(T)] = subscriberIndex;
      m_subscribers.push_back(AutoPacketSubscriber(sub));
    }

    // Prime the satisfaction graph for each element:
    for(
      auto ppCur = t_decomposition::Enumerate();
      *ppCur;
    ppCur++
      ) {
      // Obtain the decorator type at this position:
      auto r = m_decorations.find(**ppCur);
      if(r == m_decorations.end())
        // Decorator formerly not encountered, introduce it:
        r = m_decorations.insert(
        t_decMap::value_type(
        **ppCur,
        AdjacencyEntry(**ppCur)
        )
        ).first;

      // Now we need to update the adjacency entry with the new subscriber:
      r->second.subscribers.push_back(subscriberIndex);
    }
  }

  /// <summary>
  /// Registers the passed subscriber, if it defines a method called AutoFilter
  /// </summary>
  template<class T>
  typename std::enable_if<
    has_autofilter<T>::value
  >::type AddSubscriber(const std::shared_ptr<T>& sub) {
    typedef Decompose<decltype(&T::AutoFilter)> t_decomposition;

    // Cannot register a subscriber with zero arguments:
    static_assert(t_decomposition::N, "Cannot register a subscriber whose AutoFilter method is arity zero");

    // Determine whether this subscriber already exists--perhaps, it is formerly disabled
    auto q = m_subMap.find(typeid(T));
    size_t subscriberIndex;
    if(q != m_subMap.end()) {
      AutoPacketSubscriber& entry = m_subscribers[q->second];
      subscriberIndex = q->second;

      if(!entry.empty())
        // Already registered, no need to register a second time
        return;

      // Registered but previously removed, re-initialize
      entry = AutoPacketSubscriber(sub);
    }
    else {
      // Register the subscriber and pre-populate the arity:
      subscriberIndex = m_subscribers.size();
      m_subMap[typeid(T)] = subscriberIndex;
      m_subscribers.push_back(AutoPacketSubscriber(sub));
    }

    // Prime the satisfaction graph for each element:
    for(
      auto ppCur = t_decomposition::Enumerate();
      *ppCur;
    ppCur++
      ) {
      // Obtain the decorator type at this position:
      auto r = m_decorations.find(**ppCur);
      if(r == m_decorations.end())
        // Decorator formerly not encountered, introduce it:
        r = m_decorations.insert(
        t_decMap::value_type(
        **ppCur,
        AdjacencyEntry(**ppCur)
        )
        ).first;

      // Now we need to update the adjacency entry with the new subscriber:
      r->second.subscribers.push_back(subscriberIndex);
    }
  }

  template<class T>
  typename std::enable_if<
    !has_autofilter<T>::value
  >::type AddSubscriber(const std::shared_ptr<T>& sub) {}

  template<class T>
  void RemoveSubscriber(const std::shared_ptr<T>& sub) {
    auto q = m_subMap.find(typeid(T));
    if(q != m_subMap.end())
      // Clear out the matched subscriber:
      m_subscribers[q->second].ReleaseSubscriber();
  }

  /// <summary>
  /// Obtains a new packet from the object pool and configures it with the current
  /// satisfaction graph
  /// </summary>
  std::shared_ptr<AutoPacket> NewPacket(void);
};

