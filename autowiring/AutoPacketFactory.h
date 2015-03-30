// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoPacket.h"
#include "AutoFilterDescriptor.h"
#include "ContextMember.h"
#include "CoreRunnable.h"
#include "TypeRegistry.h"
#include CHRONO_HEADER
#include TYPE_TRAITS_HEADER
#include STL_UNORDERED_SET

class AutoPacketFactory;
class DispatchQueue;
class AutoPacketInternal;

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
  // Lock for this type
  mutable std::mutex m_lock;
  
  // State change notification
  std::condition_variable m_stateCondition;

  // Internal outstanding reference for issued packet:
  std::weak_ptr<void> m_outstandingInternal;
  
  // The next packet to be issued from this factory
  std::shared_ptr<AutoPacketInternal> m_nextPacket;

  // Collection of known subscribers
  typedef std::unordered_set<AutoFilterDescriptor, std::hash<AutoFilterDescriptor>> t_autoFilterSet;
  t_autoFilterSet m_autoFilters;

  // Accumulators used to compute statistics about AutoPacket lifespan.
  long long m_packetCount;
  double m_packetDurationSum;
  double m_packetDurationSqSum;

  // Returns the internal outstanding count, for use with AutoPacket
  std::shared_ptr<void> GetInternalOutstanding(void);

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

  /// <summary>
  /// Creates a linked list of saturation counters
  /// </summary>
  /// <returns>The first element in the list, or nullptr if the list is empty</returns>
  SatCounter* CreateSatCounterList(void) const;

  // CoreRunnable overrides:
  bool OnStart(void) override;
  void OnStop(bool graceful) override;
  void DoAdditionalWait(void) override;
  bool DoAdditionalWait(std::chrono::nanoseconds timeout) override;

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
  /// <param name="rhs">The descriptor for the AutoFilter to be added</param>
  /// <returns>rhs</returns>
  /// <remarks>
  /// This method is idempotent
  /// </remarks>
  const AutoFilterDescriptor& AddSubscriber(const AutoFilterDescriptor& rhs);

  /// <summary>
  /// Convenience override of AddSubscriber
  /// </summary>
  /// <param name="rhs">A shared pointer to a type which has an AutoFilter routine on it</param>
  /// <remarks>
  /// For this call to be valid, T::AutoFilter must be defined and must be a compliant AutoFilter
  /// </remarks>
  template<class T>
  AutoFilterDescriptor AddSubscriber(const std::shared_ptr<T>& rhs) {
    return AddSubscriber(AutoFilterDescriptor(rhs));
  }

  /// <summary>
  /// Removes the designated AutoFilter from this factory
  /// </summary>
  /// <param name="autoFilter">The AutoFilter to be removed</param>
  /// <remarks>
  /// This method will not retroactively modify packets that have already been issued with the specified
  /// AutoFilter on it.  Only packets that are issued after this method returns will lack the presence of
  /// the autoFilter described by the parameter.
  /// </remarks>
  void RemoveSubscriber(const AutoFilterDescriptor& autoFilter);

  /// <summary>
  /// Convienance overload of operator+= to add a subscriber from a lambda
  /// </summary>
  /// <remarks>
  /// This method provides a way to attach a lambda function directly to the factory
  /// </remarks>
  template<class Fx>
  AutoFilterDescriptor operator+= (Fx&& fx) {
    return AddSubscriber(AutoFilterDescriptor(std::forward<Fx&&>(fx)));
  }

  /// <summary>
  /// Overloaded counterpart to RemoveSubscriber
  /// </summary>
  void operator-=(const AutoFilterDescriptor& desc);

protected:
  /// <summary>
  /// Returns a description of the AutoFilter associated with the type nodeType
  /// </summary>
  /// <remarks>
  /// If a matching description was not found GetTypeDescriptor(type).GetAutoFilterTypeInfo() == nullptr
  /// </remarks>
  AutoFilterDescriptor GetTypeDescriptorUnsafe(const std::type_info* nodeType);

  static bool IsAutoPacketType(const std::type_info& dataType);

public:
  /// <summary>
  /// Obtains a new packet from the object pool and configures it with the current
  /// satisfaction graph
  /// </summary>
  std::shared_ptr<AutoPacket> NewPacket(void);

  std::shared_ptr<AutoPacketInternal> ConstructPacket(void);

  /// <returns>the number of outstanding AutoPackets</returns>
  size_t GetOutstandingPacketCount(void) const;

  /// <summary>
  /// Called by each AutoPacket's Finalize method to allow the factory
  /// to record statistics about packet lifespan.
  /// </summary>
  /// <param name="duration">
  /// The total lifetime of the AutoPacket that is being Finalized
  /// </param>
  void RecordPacketDuration(std::chrono::nanoseconds duration);

  /// <summary>
  /// Returns the number of packets which have recorded duration statistics
  /// since the most recent statistics reset.
  /// </summary>
  long long GetTotalPacketCount(void) { return m_packetCount; }

  /// <summary>
  /// Returns the mean lifespan of AutoPackets in nanoseconds since the last statistics reset.
  /// </summary>
  /// <remarks>
  /// If the factory is left to run for an extended period of time without a reset
  /// the mean will decrease erroneously due to saturation of the accumulator.
  /// </remarks>
  double GetMeanPacketLifetime(void);

  /// <summary>
  /// Returns the standard deviation of the lifespans of AutoPackets in nanoseconds since
  /// most recent statistics reset.
  /// </summary>
  /// <remarks>
  /// If the factory is left to run for an extended period of time without a reset
  /// the standard deviation will behave erraticly due to saturation of the accumulators.
  /// </remarks>
  double GetPacketLifetimeStandardDeviation(void);

  /// <summary>
  /// Resets the statistics accumulators stored by the AutoPacketFactory.
  /// </summary>
  void ResetPacketStatistics(void);
};

// Extern explicit template instantiation declarations added to prevent
// exterior instantation of internally used template instances
extern template struct SlotInformationStump<AutoPacketFactory, false>;
extern template const std::shared_ptr<AutoPacketFactory>& SharedPointerSlot::as<AutoPacketFactory>(void) const;
extern template std::shared_ptr<AutoPacketFactory> autowiring::fast_pointer_cast<AutoPacketFactory, CoreObject>(const std::shared_ptr<CoreObject>& Other);
extern template class RegType<AutoPacketFactory>;
extern template struct autowiring::fast_pointer_cast_blind<CoreObject, AutoPacketFactory>;
extern template struct autowiring::fast_pointer_cast_initializer<CoreObject, AutoPacketFactory>;
