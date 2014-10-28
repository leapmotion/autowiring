// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
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
#include STL_UNORDERED_SET

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
  mutable std::mutex m_lock;
  
  // State change notification
  std::condition_variable m_stateCondition;
  
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
    std::lock_guard<std::mutex> lk(m_lock);
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
  /// already.  This method is idempotent.
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
    AddSubscriber(MakeAutoFilterDescriptor<T>(rhs));
  }

  /// <summary>
  /// Removes the designated AutoFilter from this factory
  /// </summary>
  /// <param name="autoFilter">The AutoFilter to be removed</param>
  void RemoveSubscriber(const AutoFilterDescriptor& autoFilter);

  /// <summary>
  /// Sets the broadcast status for the specified output from the node.
  /// </summary>
  /// <remarks>
  /// When dataType = nullptr the broadcast status is set for all declared input and output data.
  /// </remarks>
  template<class node>
  void BroadcastDataOut(const std::type_info* dataType = nullptr, bool enable = true) {
    const std::type_info* nodeType = &typeid(typename SelectTypeUnifier<node>::type);
    if (dataType) {
      GetContext()->NotifyWhenAutowired<node>(
        [this, nodeType, dataType, enable](){
        BroadcastOneDataOut(nodeType, dataType, enable);
      });
    } else {
      GetContext()->NotifyWhenAutowired<node>(
        [this, nodeType, enable](){
        BroadcastAllDataOut(nodeType, enable);
      });
    }
  }

  /// <summary>
  /// Sets the broadcast status for the specified output from the node.
  /// </summary>
  /// <remarks>
  /// When dataType = nullptr the broadcast status is set for all declared input and output data.
  /// </remarks>
  template<class node>
  void BroadcastDataIn(const std::type_info* dataType = nullptr, bool enable = true) {
    const std::type_info* nodeType = &typeid(typename SelectTypeUnifier<node>::type);
    if (dataType) {
      GetContext()->NotifyWhenAutowired<node>(
        [this, nodeType, dataType, enable](){
        BroadcastOneDataIn(nodeType, dataType, enable);
      });
    } else {
      GetContext()->NotifyWhenAutowired<node>(
        [this, nodeType, enable](){
        BroadcastAllDataIn(nodeType, enable);
      });
    }
  }

  /// <summary>
  /// Establishes a data pipe from nodeIn to nodeOut.
  /// </summary>
  /// <remarks>
  /// When dataType = nullptr pipes are established for all declared data
  /// that are outputs of nodeOut and inputs to nodeIn
  /// If nodeOut includes AutoPacket& as an argument then pipes will be defined
  /// for all declared input types of nodeIn. Likewise, if nodeIn declares
  /// AutoPacket& or const AutoPacket& as an argument then pipes will be defined
  /// for all declared outputs of nodeOut.
  /// </remarks>
  template<class nodeOut, class nodeIn>
  void PipeData(const std::type_info* dataType = nullptr, bool enable = true) {
    const std::type_info* nodeOutType = &typeid(typename SelectTypeUnifier<nodeOut>::type);
    const std::type_info* nodeInType = &typeid(typename SelectTypeUnifier<nodeIn>::type);
    if (dataType) {
      GetContext()->NotifyWhenAutowired<nodeOut>(
        [this, nodeOutType, nodeInType, dataType, enable](){
        GetContext()->NotifyWhenAutowired<nodeIn>(
        [this, nodeOutType, nodeInType, dataType, enable](){
          PipeOneData(nodeOutType, nodeInType, dataType, enable);
        });
      });
    } else {
      GetContext()->NotifyWhenAutowired<nodeOut>(
        [this, nodeOutType, nodeInType, enable](){
        GetContext()->NotifyWhenAutowired<nodeIn>(
        [this, nodeOutType, nodeInType, enable](){
          PipeAllData(nodeOutType, nodeInType, enable);
        });
      });
    }
  }

protected:
  /// <summary>
  /// Returns a description of the AutoFilter associated with the type nodeType
  /// </summary>
  /// <remarks>
  /// If a matching description was not found GetTypeDescriptor(type).GetAutoFilterTypeInfo() == nullptr
  /// </remarks>
  AutoFilterDescriptor GetTypeDescriptorUnsafe(const std::type_info* nodeType);

  void BroadcastOneDataIn(const std::type_info* nodeType, const std::type_info* dataType, bool enable);
  void BroadcastAllDataIn(const std::type_info* nodeType, bool enable);

  void BroadcastOneDataOut(const std::type_info* nodeType, const std::type_info* dataType, bool enable);
  void BroadcastAllDataOut(const std::type_info* nodeType, bool enable);

  void PipeOneData(const std::type_info* nodeOutType, const std::type_info* nodeInType, const std::type_info* dataType, bool enable);
  void PipeAllData(const std::type_info* nodeOutType, const std::type_info* nodeInType, bool enable);

  static bool IsAutoPacketType(const std::type_info& dataType) {
    return
    dataType == typeid(AutoPacket) ||
    dataType == typeid(auto_arg<AutoPacket&>::id_type) ||
    dataType == typeid(auto_arg<const AutoPacket&>::id_type);
  }

public:
  /// <summary>
  /// Obtains a new packet from the object pool and configures it with the current
  /// satisfaction graph
  /// </summary>
  std::shared_ptr<AutoPacket> NewPacket(void);

  /// <returns>the number of outstanding AutoPackets</returns>
  size_t GetOutstanding(void) const { return m_packets.GetOutstanding(); }
};

// Extern explicit template instantiation declarations added to prevent
// exterior instantation of internally used template instances
extern template class RegType<AutoPacketFactory>;
extern template struct SlotInformationStump<AutoPacketFactory, false>;
extern template const std::shared_ptr<AutoPacketFactory>& SharedPointerSlot::as<AutoPacketFactory>(void) const;
extern template class std::shared_ptr<AutoPacketFactory>;
