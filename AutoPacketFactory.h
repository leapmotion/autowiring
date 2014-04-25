#pragma once
#include "auto_out.h"
#include "Autowired.h"
#include "AutoPacket.h"
#include "AutoPacketSubscriber.h"
#include "Decompose.h"
#include "FilterPropertyExtractor.h"
#include "ObjectPool.h"
#include <boost/any.hpp>
#include <typeinfo>
#include <vector>
#include TYPE_INDEX_HEADER
#include TYPE_TRAITS_HEADER
#include STL_UNORDERED_MAP

class AutoPacketListener;
class Deferred;
class DispatchQueue;

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

    // Indexes into the subscriber satisfaction vector.  Each entry in this list represents a single
    // subscriber, and an offset in the m_subscribers vector.  The second element in the pair is the
    // optional flag.
    std::vector<std::pair<size_t, bool>> subscribers;
  };

  AutoPacketFactory(void);

  /// <summary>
  /// Utility ctor, provided to allow AutoPacketFactory construction during CoreContext construction
  /// </summary>
  AutoPacketFactory(std::shared_ptr<JunctionBox<AutoPacketListener>>&& apl);

  ~AutoPacketFactory(void);

private:
  struct AutoPacketResetter {
    AutoPacketResetter(void);

    AutoPacketResetter(AutoPacketResetter&& rhs);

    AutoPacketResetter(AutoFired<AutoPacketListener>&& apl);

    AutoFired<AutoPacketListener> m_apl;

    void operator()(AutoPacket& packet) const;
  };

  struct AutoPacketCreator {
    AutoPacketCreator(AutoPacketFactory* factory = nullptr) :
      factory(factory)
    {}

    AutoPacketCreator(AutoPacketCreator&& rhs) :
      factory(rhs.factory)
    {}

    AutoPacketFactory* factory;

    AutoPacket* operator()() const;

    void operator=(AutoPacketCreator&& rhs) {
      factory = rhs.factory;
    }
  };

  /// <summary>
  /// An independently maintained object pool just for packets
  /// </summary>
  /// <remarks>
  /// The object pool defined here is provided mainly to allow detection of
  /// pipeline packet expiration in order to support expiration notification
  /// broadcasts.
  /// </remarks>
  ObjectPool<AutoPacket, AutoPacketResetter, AutoPacketCreator> m_packets;

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

  // Utility override, does nothing
  void AddSubscriber(std::false_type) {}

  std::function<void()> fn;

public:
  // Accessor methods:
  const std::vector<AutoPacketSubscriber>& GetSubscriberVector(void) const { return m_subscribers; }
  const t_decMap& GetDecorations(void) const { return m_decorations; }

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

  template<class T>
  const AutoPacketSubscriber* FindSubscriber(void) const { return FindSubscriber(typeid(T)); }

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
  void AddSubscriber(const AutoPacketSubscriber& rhs);

  /// <summary>
  /// Convenience override of AddSubscriber
  /// </summary>
  template<class T>
  void AddSubscriber(const std::shared_ptr<T>& rhs) {
    AddSubscriber(AutoPacketSubscriberSelect<T>(rhs));
  }

  void RemoveSubscriber(const std::type_info &idx);

  template<class T>
  void RemoveSubscriber(const std::shared_ptr<T>& sub) {
    RemoveSubscriber(typeid(T));
  }

  void RemoveSubscribers( std::vector<AutoPacketSubscriber>::const_iterator first, std::vector<AutoPacketSubscriber>::const_iterator last) {
    for( auto i = first; i != last; i++ ) {
      RemoveSubscriber( *i->GetSubscriberTypeInfo() );
    }
  }

  /// <returns>
  /// True if the indicated type has been requested for use by some consumer
  /// </returns>
  /// <remarks>
  /// This method is used to determine whether an AutoFilter consumer exists for the
  /// specified type at the time of the call.  Note that this method may occassionally
  /// return an incorrect result in a multithreaded context.
  /// </remarks>
  template<class T>
  bool HasSubscribers(void) const { return HasSubscribers(typeid(T)); }

  bool HasSubscribers(const std::type_info& ti) const;

  /// <summary>
  /// Obtains a new packet from the object pool and configures it with the current
  /// satisfaction graph
  /// </summary>
  std::shared_ptr<AutoPacket> NewPacket(void);
};

