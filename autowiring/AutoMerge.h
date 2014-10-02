// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <autowiring/AutoPacket.h>
#include <unordered_set>

/// <summary>
/// Extracts all instances of a type from a slave-context on each execution of that context.
/// </summary>
/// <remarks>
/// It is expected that the merged data is desired in another context.
/// This process is facilitated the AutoStile and AutoMergeStile.
/// </remarks>
template<class merge_type>
class AutoMerge
{
protected:
  Autowired<AutoPacketFactory> m_factory;

public:
  typedef std::unordered_map<std::type_index, std::shared_ptr<merge_type>> merge_data;
  typedef std::function<void(const merge_data&)> merge_call;

  /// <summary>Final call in slave context to extract all data of the specified type</summary>
  /// <remarks>
  /// This will only gather data that is directed to this source, identified by the gather type.
  /// </remarks>
  void AutoFilter(const AutoPacket& packet, const merge_call& call) {
    if (!call)
      return;

    // Gather relevant data of the specified type
    merge_data unordered = packet.GetAll<merge_type>(typeid(typename SelectTypeUnifier<AutoMerge<merge_type>>::type));
    merge_data broadcast = packet.GetAll<merge_type>(typeid(void));
    unordered.insert(broadcast.begin(), broadcast.end());

    // Merge with prior merged data
    if (packet.Has<merge_data>()) {
      merge_data priordata = packet.Get<merge_data>();
      unordered.insert(priordata.begin(), priordata.end());
    }

    // Call the master function decorating this packet
    call(unordered);
  }

  /// <summary>Enables a pipe from source to AutoMerge, and disables broadcasting by source.</summary>
  /// <param name="enable">Removes pipe and reinstates broadcast when false</param>
  /// <remarks>
  /// Parameters have the same meaning as in AutoPacketFactory PipeData and BroadcastData
  /// </remarks>
  template<class source>
  void PipeToMerge(const std::type_info* dataType = nullptr, bool enable = true) {
    if (!m_factory)
      return;
    m_factory->BroadcastDataOut<source>(dataType, !enable);
    m_factory->PipeData<source, AutoMerge<merge_type>>(dataType, enable);
  }
};

/// <summary>
/// This provides a merge extractor function that can be injected in to a slave_context by AutoStile,
/// and constructs a corresponding instance of AutoMerge in the slave_context.
/// </summary>
/// <remarks>
/// In order for the merged data to be extracted, it is necessary that AutoStile include
/// AutoMergeStile::merge_call as an *input*, so that it will be injected.
/// </remarks>
template<class merge_type>
class AutoMergeStile {
protected:
  std::weak_ptr<AutoMerge<merge_type>> m_merge;

public:
  typedef typename AutoMerge<merge_type>::merge_data merge_data;
  typedef typename AutoMerge<merge_type>::merge_call merge_call;

  AutoMergeStile(CoreContext& slave_context) {
    CurrentContextPusher pusher(&slave_context);
    m_merge = AutoRequired<AutoMerge<merge_type>>();
  }

  /// <summary>Provides an extractor function that can be injected by AutoStile</summary>
  void AutoFilter(AutoPacket& packet) {
    // IMPORTANT:
    // Because the packet reference will be a decoration on packet,
    // if it holds a shared_ptr to itself it will never terminate.
    std::weak_ptr<AutoPacket> weak_packet = packet.shared_from_this();
    packet.Decorate(merge_call([weak_packet](const merge_data& unordered){
      std::shared_ptr<AutoPacket> master_packet = weak_packet.lock();
      if (!master_packet)
        return;
      master_packet->Decorate(unordered);
    }));
  }

  std::shared_ptr<AutoMerge<merge_type>> GetMerge() {
    return m_merge.lock();
  }
};
