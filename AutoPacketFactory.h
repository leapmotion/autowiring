#pragma once
#include "Autowired.h"
#include "AutoPacket.h"
#include "AutoFilterDescriptor.h"
#include "ContextMember.h"
#include "CoreRunnable.h"
#include "ObjectPool.h"
#include <list>
#include <vector>
#include TYPE_INDEX_HEADER
#include TYPE_TRAITS_HEADER
#include STL_UNORDERED_MAP

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
  public ContextMember,
  public CoreRunnable
{
public:
  AutoPacketFactory(void);
  ~AutoPacketFactory(void);

private:
  // Parent packet factory, if one exists:
  Autowired<AutoPacketFactory> m_parent;

  // Lock for this type
  mutable boost::mutex m_lock;
  
  // State change notification
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
  ObjectPool<AutoPacket> m_packets;

  // Collection of known subscribers
  typedef std::unordered_set<AutoFilterDescriptor, std::hash<AutoFilterDescriptor>> t_autoFilterSet;
  t_autoFilterSet m_autoFilters;

  // Recursive invalidation routine, causes AutoPacket object pools to be dumped to the root
  void Invalidate(void);

  // Utility override, does nothing
  void AddSubscriber(std::false_type) {}

public:
  /// <summary>
  /// Copies the internal set of AutoFilter members to the specified container
  /// </summary>
  template<class T>
  void AppendAutoFiltersTo(T& container) const {
    boost::lock_guard<boost::mutex> lk(m_lock);
    container.insert(container.end(), m_autoFilters.begin(), m_autoFilters.end());
  }

  // CoreRunnable overrides:
  bool Start(std::shared_ptr<Object> outstanding) override;
  void Stop(bool graceful = false) override;
  void Wait(void) override;
  bool IsRunning(void) const override { return m_outstanding && !m_wasStopped; };
  bool ShouldStop(void) const override { return m_wasStopped; };

  /// <summary>
  /// Causes this AutoPacketFactory to release all of its packet subscribers
  /// </summary>
  /// <remarks>
  /// This method will cause the factory to enter the Stopped state, if it's not there
  /// alread.  This method is idempotent.
  /// </remarks>
  void Clear(void);

  /// <summary>
  /// Registers the passed subscriber, if it defines a method called AutoFilter
  /// </summary>
  /// <remarks>
  /// This method is idempotent
  /// </remarks>
  void AddSubscriber(const AutoFilterDescriptor& rhs);

  /// <summary>
  /// Convenience override of AddSubscriber
  /// </summary>
  template<class T>
  void AddSubscriber(const std::shared_ptr<T>& rhs) {
    AddSubscriber(AutoFilterDescriptorSelect<T>(rhs));
  }

  /// <summary>
  /// Removes the designated AutoFilter from this factory
  /// </summary>
  /// <param name="autoFilter">The AutoFilter to be removed</param>
  void RemoveSubscriber(const AutoFilterDescriptor& autoFilter);

  /// <summary>
  /// Obtains a new packet from the object pool and configures it with the current
  /// satisfaction graph
  /// </summary>
  std::shared_ptr<AutoPacket> NewPacket(void);
};
