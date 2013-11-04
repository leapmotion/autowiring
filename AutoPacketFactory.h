#pragma once
#include "Decompose.h"
#include "FilterPropertyExtractor.h"
#include "ObjectPool.h"
#include <typeindex>
#include STL_UNORDERED_MAP

class AutoPacket;

/// <summary>
/// Subscriber wrap, represents a single logical subscriber
/// </summary>
class AutoPacketSubscriber {
public:
  typedef void (*t_call)(void*, const AutoPacket&);

  AutoPacketSubscriber(size_t subscriberIndex = -1) :
    m_subscriberIndex(subscriberIndex)
  {}

  AutoPacketSubscriber(const AutoPacketSubscriber& rhs) :
    m_subscriberIndex(rhs.m_subscriberIndex),
    m_arity(rhs.m_arity),
    m_pObj(rhs.m_pObj),
    m_pCall(rhs.m_pCall)
  {}

  /// <summary>
  /// Constructs a new packet subscriber entry based on the specified subscriber
  /// </summary>
  /// <remarks>
  /// This constructor increments the reference count on the passed object until the
  /// object is freed.  A subscriber wraps the templated type, automatically mapping
  /// desired arguments into the correct locations, via use of Decompose::Call and
  /// a AutoPacket to provide type sources
  /// </summary>
  template<class T>
  AutoPacketSubscriber(std::shared_ptr<T> subscriber):
    m_arity(Decompose<decltype(&T::AutoFilter)>::N),
    m_pObj(subscriber.get()),
    m_pCall(
      reinterpret_cast<t_call>(
        &Decompose<decltype(&T::AutoFilter)>::Call<AutoPacket, &T::AutoFilter>
      )
    )
  {}

protected:
  size_t m_subscriberIndex;

  // The number of parameters that will be extracted from the repository object when making
  // a Call.  This is used to prime the AutoPacket in order to make saturation checking work
  // correctly.
  size_t m_arity;

  // This is the first argument that will be passed to the Call function defined below.  It
  // is a pointer to an actual subscriber, but with type information omitted for performance.
  void* m_pObj;

  // The first argument of this static global is void*, but it is expected that the argument
  // that will actually be passed is of a type corresponding to the member function bound
  // by this operation.  Strong guarantees must be made that the types
  t_call m_pCall;

public:
  // Accessor methods:
  size_t GetArity(void) const { return m_arity; }
  size_t GetSubscriberIndex(void) const { return m_subscriberIndex; }

  /// <returns>A pointer to the subscriber</returns>
  void* GetSubscriber(void) const { return m_pObj; }

  /// <returns>A call lambda wrapping the associated subscriber</returns>
  /// <remarks>
  /// Parameters for the associated subscriber are obtained by querying the packet.
  /// The packet must already be decorated with all required parameters for the
  /// subscribers, or an exception will be thrown.
  /// </remarks>
  t_call GetCall(void) const { return m_pCall; }
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
    std::list<size_t> subscribers;
  };

  AutoPacketFactory(void);
  ~AutoPacketFactory(void);

private:
  /// <summary>
  /// An independently maintained object pool just for packets
  /// </summary>
  /// <remarks>
  /// The object pool defined here is provided mainly to allow detection of
  /// pipeline packet expiration in order to support expiration notification
  /// broadcasts.
  /// </remarks>
  ObjectPool<AutoPacket> m_packets;

  // Optimization value.  Stores the total number of all satisfactions.  This
  // is equivalent to the sum of all of the sizes in the m_sats list.
  size_t m_numSats;

  // Vector of known subscribers--a vector must be used because random access is
  // required due to its use in an adjacency list.
  std::vector<AutoPacketSubscriber> m_subscribers;

  // Map used to associate a decorator type with the adjacency entries for that type.
  typedef std::unordered_map<std::type_index, AdjacencyEntry> t_decMap;
  t_decMap m_decorations;

  // Map used to associate a subscriber type with a monotonic index
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
    if(subscriberIndex == -1)
      return nullptr;
    return &m_subscribers[subscriberIndex];
  }

  /// <summary>
  /// Finds the monotonic index corresponding to a particular subscriber type
  /// </summary>
  /// <returns>The index, or -1 if the subscriber does not exist</returns>
  size_t FindSubscriberIndex(const std::type_info& info) const {
    auto q = m_subMap.find(info);
    return q == m_subMap.end() ? -1 : q->second;
  }

  /// <summary>
  /// Finds the entry in the adjacency list corresponding to a decorator type
  /// </summary>
  /// <returns>The adjacency entry, or nullptr if no such entry exists</returns>
  const AdjacencyEntry* FindDecorator(const std::type_info& info) const {
    auto q = m_decorations.find(info);
    return q == m_decorations.end() ? nullptr : &q->second;
  }

  /// <summary>
  /// Registers the passed subscriber, if it defines a method called AutoFilter
  /// </summary>
  template<class T>
  typename std::enable_if<
    has_autofilter<T>::value
  >::type AddSubscriber(const std::shared_ptr<T>& sub) {
    typedef Decompose<decltype(&T::AutoFilter)> t_decomposition;

    // Find the subscriber we're adding:
    auto q = m_subMap.find(typeid(T));
    if(q != m_subMap.end())
      // Already registered, no need to register a second time
      return;

    // Cannot register a subscriber with zero arguments:
    const size_t arity = t_decomposition::N;
    static_assert(0 != arity, "Cannot register a subscriber whose AutoFilter method is arity zero");

    // Register the subscriber and pre-populate the arity:
    size_t subscriberIndex = m_subscribers.size();
    m_subMap[typeid(T)] = subscriberIndex;
    m_subscribers.push_back(AutoPacketSubscriber(sub));

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

  /// <summary>
  /// Obtains a new packet from the object pool and configures it with the current
  /// satisfaction graph
  /// </summary>
  std::shared_ptr<AutoPacket> NewPacket(void);
};

