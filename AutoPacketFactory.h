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
  struct AutoPacketCreator {
    AutoPacketCreator(AutoPacketFactory* factory = nullptr) : factory(factory) {}

    AutoPacketFactory* factory;

    AutoPacket* operator()() const;
    void operator=(AutoPacketCreator&& rhs) { factory = rhs.factory; }
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
  ObjectPool<AutoPacket, NoOp, AutoPacketCreator> m_packets;

  // Collection of known subscribers
  typedef std::unordered_set<AutoPacketSubscriber> t_autoFilterSet;
  t_autoFilterSet m_autoFilters;

  struct DecorationEntry {
    // The single statically detected publisher of the outer type
    AutoPacketSubscriber m_publisher;

    // All subscribers on this entry
    std::vector<AutoPacketSubscriber> m_subscribers;
  };

  // Map used to associate a decoration type with the subscribers of that type.
  typedef std::unordered_map<std::type_index, DecorationEntry> t_decorationMap;
  t_decorationMap m_decorationMap;

  // Utility override, does nothing
  void AddSubscriber(std::false_type) {}

public:
  // Accessor methods:
  const t_autoFilterSet& GetSubscriberVector(void) const { return m_autoFilters; }
  const t_decorationMap& GetDecorations(void) const { return m_decorationMap; }

  // CoreRunnable overrides:
  bool Start(std::shared_ptr<Object> outstanding) override;
  void Stop(bool graceful = false) override;
  void Wait(void) override;
  bool IsRunning(void) const override { return m_outstanding && !m_wasStopped; };
  bool ShouldStop(void) const override { return m_wasStopped; };

  /// <summary>
  /// Registers the passed subscriber, if it defines a method called AutoFilter
  /// </summary>
  /// <remarks>
  /// This method is idempotent
  /// </remarks>
  void AddSubscriber(const AutoPacketSubscriber& rhs);

  /// <summary>
  /// Convenience override of AddSubscriber
  /// </summary>
  template<class T>
  void AddSubscriber(const std::shared_ptr<T>& rhs) {
    AddSubscriber(AutoPacketSubscriberSelect<T>(rhs));
  }

  /// <summary>
  /// Removes the designated AutoFilter from this factory
  /// </summary>
  /// <param name="autoFilter">The AutoFilter to be removed</param>
  void RemoveSubscriber(const AutoPacketSubscriber& autoFilter);

  /// <summary>
  /// Obtains a new packet from the object pool and configures it with the current
  /// satisfaction graph
  /// </summary>
  std::shared_ptr<AutoPacket> NewPacket(void);
};

