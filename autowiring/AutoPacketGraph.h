// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoFilterDescriptor.h"
#include "AutoPacket.h"
#include "AutoPacketFactory.h"
#include "Autowired.h"
#include "AutowiringEvents.h"
#include "CoreRunnable.h"
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
class AutoPacketGraph:
  public AutowiringEvents,
  public CoreRunnable
{
public:
  AutoPacketGraph();
  
  typedef std::unordered_map<DeliveryEdge, size_t, std::hash<DeliveryEdge>> t_deliveryEdges;
  
protected:
  // A mapping of an edge to the number of times it was delivered
  t_deliveryEdges m_deliveryGraph;
  
  // A lock for this type
  mutable std::mutex m_lock;
  
  // Reference to the AutoPacketFactory
  AutoRequired<AutoPacketFactory> m_factory;
  
  /// <summary>
  /// Scan all of the objects and add any AutoFilter's from all of the objects in a system.
  /// </summary>
  /// <remarks>
  /// This function will scan all of the objects (and rescan) and only add new edges to our graph.
  /// </remarks>
  void LoadEdges();
  
  /// <summary>
  /// Record the delivery of a packet and increment the number of times the packet has been delivered
  /// </summary>
  void RecordDelivery(const std::type_info* ti, const AutoFilterDescriptor& descriptor, bool input);
  
  /// AutowiringEvents overrides
  virtual void NewContext(CoreContext&) {}
  virtual void ExpiredContext(CoreContext&) {}
  virtual void EventFired(CoreContext&, const std::type_info&) {}
  virtual void NewObject(CoreContext&, const ObjectTraits&);
  
  /// CoreRunnable overrides
  virtual bool DoStart(void);
  
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
