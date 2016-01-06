// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoFilterDescriptor.h"
#include "AutoPacket.h"
#include "AutoPacketFactory.h"
#include "Autowired.h"
#include "AutowiringEvents.h"
#include "CoreRunnable.h"
#include STL_UNORDERED_MAP


/// \internal
/// <summary>
/// Represents an edge in the graph from a type to an AutoFilter
/// </summary>
struct DeliveryEdge
{
  enum class ArgType {
    Input,
    Output,
    Rvalue
  };

  // The type info
  auto_id type_info;
  
  // The AutoFilterDescriptor
  AutoFilterDescriptor descriptor;
  
  // Specifies if the argument is an input (type -> descriptor) or output (descriptor -> type)
  // or rvalue (type <-> descriptor)
  ArgType arg_type;
  
  // For the unordered map/hash comparison
  bool operator==(const DeliveryEdge& rhs) const {
    return
      type_info == rhs.type_info &&
      descriptor == rhs.descriptor &&
      arg_type == rhs.arg_type;
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
      return (size_t) edge.descriptor.GetAutoFilter().ptr();
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
  typedef std::unordered_map<DeliveryEdge, size_t, std::hash<DeliveryEdge>> t_deliveryEdges;
  
protected:
  // A mapping of an edge to the number of times it was delivered
  t_deliveryEdges m_deliveryGraph;
  
  // A lock for this type
  mutable std::mutex m_lock;
  
  // Reference to the AutoPacketFactory
  AutoRequired<AutoPacketFactory> m_factory;
  
  /// <summary>
  /// Demangle a type name as well as stripping "auto_in< >"
  /// </summary>
  /// <remarks>
  /// The ">" that encloses the templates will have extra spaces between them, for instance
  ///
  ///    auto_in<Class>
  ///    auto_in<Class<T> >
  ///    auto_in<Class1<Class2, Class3<Class4> > >
  ///
  /// All we care about is matching "^auto_in<(.*)>$"
  /// </remarks>
  std::string DemangleTypeName(const std::type_info* type_info) const;
  
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
  void RecordDelivery(auto_id id, const AutoFilterDescriptor& descriptor, DeliveryEdge::ArgType arg_type);
  
  /// AutowiringEvents overrides
  virtual void NewContext(CoreContext&) override {}
  virtual void ExpiredContext(CoreContext&) override {}
  virtual void NewObject(CoreContext&, const CoreObjectDescriptor&) override;
  
  /// CoreRunnable overrides
  virtual bool OnStart(void) override;
  
public:
  /// <summary>
  /// Get a copy of the packet via AutoFilter
  /// </summary>
  void AutoFilter(AutoPacket& packet);
  
  /// <summary>
  /// Write the graph to a file in graphviz format
  /// </summary>
  /// <param name="filename">
  /// The name of the file to write the graph to
  /// </param>
  /// <param name="numPackets">
  /// Include the number of times the packet was delivered
  /// </param>
  bool WriteGV(const std::string& filename, bool numPackets = false) const;
};
