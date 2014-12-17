// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoPacketGraph.h"
#include "AutoPacketProfiler.h"
#include "demangle.h"
#include <fstream>

AutoPacketGraph::AutoPacketGraph()
{
}


void AutoPacketGraph::AddEdge(const std::type_info* ti, const AutoFilterDescriptor& descriptor, bool input) {
  DeliveryEdge edge { ti, descriptor, input };
  
  std::lock_guard<std::mutex> lk(m_lock);
  auto itr = m_deliveryGraph.find(edge);
  if (itr == m_deliveryGraph.end()) {
    m_deliveryGraph[edge] = 1;
  } else {
    itr->second++;
  }
}

bool AutoPacketGraph::WriteGV(const std::string& filename) const
{
  std::ofstream file(filename);
  if (!file && !file.good()) {
    return false;
  }
  
  file << "digraph AutoPacketGraph {\n";
  
  std::lock_guard<std::mutex> lk(m_lock);
  for (auto& itr : m_deliveryGraph) {
    const DeliveryEdge& edge = itr.first;
    size_t count = itr.second;
    
    // string format: "type" -> "AutoFilter" (or vice versa)
    std::stringstream ss;
    ss << "  \"";
    if (edge.input) {
      ss << autowiring::demangle(edge.type_info) << "\" -> \"" << autowiring::demangle(edge.descriptor.GetType());
    } else {
      ss << autowiring::demangle(edge.descriptor.GetType()) << "\" -> \"" << autowiring::demangle(edge.type_info);
    }
    
    // TODO: count should probably be optional
    ss << "\" [label=\"" << count << "\"];" << std::endl;
    
    file << ss.str();
  }
  
  file << "}\n";
  
  return true;
}
