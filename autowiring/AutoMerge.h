// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <autowiring/AutoPacket.h>
#include <unordered_set>

/// <summary>
/// Extracts all instances of a type from a slave-context on each execution of that context.
/// </summary>
/// <remarks>
/// It is expected that the merged data is desired in another context.
/// Let MasterFilter exist in the Master context, and let AutoMerge exist in the Slave context.
/// When MasterFilter::AutoFilter(AutoPacket& packet, ...) is called in the Master context it will
/// initiate execution of the Slave context as follows:
///   std::shared_ptr<AutoPacket> master_packet = packet.shared_from_this();
///   {
///     std::shared_ptr<AutoPacket> slave_packet = slave_factory->NewPacket();
///     slave_packet.Decorate([master_packet] (const AutoMerge<merge_type>::merge_data& data)
///       master_packet.Decorate(data);
///     )
///   }
/// This ensures that before Finalize is called on master_packet it will be decorated with the
/// merged data from slave_context. Because the Slave context may include Deferred AutoFilter
/// methods, it cannot be guaranteed that the data will be immediately available.
/// </remarks>
template<class merge_type>
class AutoMerge
{
public:
  typedef std::unordered_map<std::type_index, std::shared_ptr<merge_type>> merge_data;
  typedef std::function<void(const merge_data&)> merge_call;

  /// <summary>Final call in slave context to extract all data of the specified type</summary>
  /// <remarks>
  /// This will only gather data that is directed to this source, identified by the gather type.
  /// </remarks>
  void AutoFilter(const AutoPacket& slave_data, const std::function<void(const merge_data&)>& master_call) {
    if (!master_call)
      return;

    // Gather relevant data of the specified type
    merge_data unordered = slave_data.GetAll<merge_type>(typeid(AutoMerge<merge_type>));
    merge_data broadcast = slave_data.GetAll<merge_type>(typeid(void));
    unordered.insert(broadcast.begin(), broadcast.end());

    // Call the master function decorating this packet
    master_call(unordered);
  }
};
