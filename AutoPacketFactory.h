#pragma once
#include "Autowired.h"
#include "AutoPacket.h"
#include "AutoPacketSubscriber.h"
#include "CoreRunnable.h"
#include "Object.h"
#include "ObjectPool.h"
#include <vector>
#include TYPE_INDEX_HEADER
#include TYPE_TRAITS_HEADER
#include STL_UNORDERED_MAP

class AutoPacketListener;
class Deferred;
class DispatchQueue;
struct AdjacencyEntry;

/// <summary>
/// A configurable factory class for pipeline packets with a built-in object pool
/// </summary>
/// <remarks>
/// Generally, only one packet factory is required per context.
/// </remarks>
class AutoPacketFactory:
  public Object,
  public CoreRunnable
{
public:
  AutoPacketFactory(void);
  ~AutoPacketFactory(void);

private:
  struct AutoPacketResetter {
    AutoPacketResetter(void);

    AutoPacketResetter(AutoPacketResetter&& rhs);

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

  // Lock for this type
  mutable boost::mutex m_lock;
  
  // Notify when started
  boost::condition_variable m_stateCondition;
  
  // Have we been signaled to stop
  bool m_wasStopped;

  // Outstanding reference if this factory is currently running:
  std::shared_ptr<Object> m_outstanding;

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

public:
  // Accessor methods:
  const std::vector<AutoPacketSubscriber>& GetSubscriberVector(void) const { return m_subscribers; }
  const t_decMap& GetDecorations(void) const { return m_decorations; }

  // CoreRunnable overrides:
  bool Start(std::shared_ptr<Object> outstanding) override;
  void Stop(bool graceful = false) override;
  void Wait(void) override;
  bool IsRunning(void) const override { return m_outstanding && !m_wasStopped; };
  bool ShouldStop(void) const override { return m_wasStopped; };

  /// <summary>
  /// Finds the entry in the adjacency list corresponding to a decorator type
  /// </summary>
  /// <returns>The adjacency entry, or nullptr if no such entry exists</returns>
  const AdjacencyEntry* FindDecoration(const std::type_info& info) const {
    boost::lock_guard<boost::mutex> lk(m_lock);
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

