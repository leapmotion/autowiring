// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoFilterDescriptor.h"
#include "AutoPacket.h"
#include "AutoPacketFactory.h"
#include "Autowired.h"
#include STL_UNORDERED_MAP


/// <summary>
/// Represents an edge in the graph from a type to an AutoFilter
/// </summary>
struct DeliveryEdge
{
  // The type info
  const std::type_info* type_info;
  
  // The AutoFilterDescriptor
  AutoFilterDescriptor descriptor;
  
  // Specifies if the argument is an input (type -> descriptor) or output (descriptor -> type)
  bool input;
  
  // For the unordered map/hash comparison
  bool operator==(const DeliveryEdge& rhs) const {
    return
      type_info == rhs.type_info &&
      descriptor == rhs.descriptor &&
      input == rhs.input;
  }
};

/// <summary>
/// Using the same hash function as the AutoFilterDescriptor
/// </summary>
namespace std {
  template<>
  struct hash<DeliveryEdge>
  {
    size_t operator()(const DeliveryEdge& edge) const {
      return (size_t) edge.descriptor.GetAutoFilter()->ptr();
    }
  };
}

/// <summary>
/// Graphical visualization of AutoPackets
/// </summary>
class AutoPacketGraph
{
public:
  AutoPacketGraph();
  
protected:
  // A mapping of an edge to the number of times it was delivered
  typedef std::unordered_map<DeliveryEdge, size_t, std::hash<DeliveryEdge>> t_deliveryEdges;
  t_deliveryEdges m_deliveryGraph;
  
  // A lock for this type
  mutable std::mutex m_lock;
  
  // Reference to the AutoPacketFactory
  Autowired<AutoPacketFactory> m_factory;
  
  /// <summary>
  /// Add an edge to the graph given the following parameters
  /// </summary>
  void AddEdge(const std::type_info* ti, const AutoFilterDescriptor& descriptor, bool input);
  
public:
  /// <summary>
  /// Get a copy of the packet via AutoFilter
  /// </summary>
  void AutoFilter(AutoPacket& packet);
  
  /// <summary>
  /// Get a mapping of the DeliveryEdge to the number of times the AutoFilter was called
  /// </summary>
  t_deliveryEdges GetEdgeCounts() const;
  
  /// <summary>
  /// Write the graph to a file in graphviz format
  /// </summary>
  bool WriteGV(const std::string& filename) const;
};
