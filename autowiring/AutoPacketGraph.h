// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoFilterDescriptor.h"
#include "SatCounter.h"
#include STL_UNORDERED_MAP



/// <summary>
/// TODO
/// </summary>
struct DeliveryEdge
{
  // The type info
  const std::type_info* type_info;
  
  //
  AutoFilterDescriptor descriptor;
  
  // Specifies if the argument is an
  bool input;
  
  bool operator==(const DeliveryEdge& rhs) const {
    // AutoFilter methods are the same for all instances of a class,
    // and classes may have more than one AutoFilter method,
    // so both comparisons are required.
    return
      type_info == rhs.type_info &&
      descriptor == rhs.descriptor &&
      input == rhs.input;
  }
};

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
/// TODO
/// </summary>
class AutoPacketGraph
{
public:
  AutoPacketGraph();
  
  bool ExportGV(const std::string& filename) const;
  
  void RecordDelivery(const std::type_info* ti, const AutoFilterDescriptor& descriptor, bool input);
  
protected:
  // A mapping
  std::unordered_map<DeliveryEdge, size_t, std::hash<DeliveryEdge>> m_deliveryGraph;
};
