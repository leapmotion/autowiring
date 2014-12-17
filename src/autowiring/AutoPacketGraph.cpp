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

void AutoPacketGraph::AutoFilter(AutoPacket& packet) {
  packet.AddTeardownListener([this, &packet] () {
    for (auto& decoration : packet.GetDispositions()) {
      if (decoration.m_publisher) {
        AddEdge(decoration.m_type, *decoration.m_publisher, false);
      }
      
      for (auto& subscriber : decoration.m_subscribers) {
        if (subscriber->called) {
          AddEdge(decoration.m_type, *subscriber, true);
        }
      }
    }
  });
}

bool AutoPacketGraph::WriteGV(const std::string& filename) const
{
  std::ofstream file(filename);
  if (!file && !file.good()) {
    return false;
  }
  
  file << "digraph AutoPacketGraph {\n";
  
  std::lock_guard<std::mutex> lk(m_lock);
  
  // Containers for the unique types and descriptors
  std::unordered_set<const std::type_info*> types;
  std::unordered_set<AutoFilterDescriptor, std::hash<AutoFilterDescriptor>> descriptors;
  
  // draw the edges
  for (auto& itr : m_deliveryGraph) {
    auto& edge = itr.first;
    auto type = edge.type_info;
    auto& descriptor = edge.descriptor;
    auto count = itr.second;
    
    if (types.find(type) == types.end()) {
      types.insert(type);
    }
    
    if (descriptors.find(descriptor) == descriptors.end()) {
      descriptors.insert(descriptor);
    }
    
    // string format: "type" -> "AutoFilter" (or vice versa)
    std::stringstream ss;
    ss << "  \"";
    if (edge.input) {
      ss << autowiring::demangle(type) << "\" -> \"" << autowiring::demangle(descriptor.GetType());
    } else {
      ss << autowiring::demangle(descriptor.GetType()) << "\" -> \"" << autowiring::demangle(type);
    }
    
    // TODO: count should probably be optional
    ss << "\" [label=\"" << count << "\"];" << std::endl;
    
    file << ss.str();
  }
  file << std::endl;
  
  // Setup the shapes for the types and descriptors
  for (auto& type : types)
    file << "  \"" << autowiring::demangle(type) << "\" [shape=box];" << std::endl;
  
  for (auto& descriptor : descriptors)
    file << "  \"" << autowiring::demangle(descriptor.GetType()) << "\" [shape=ellipse];" << std::endl;
  
  file << "}\n";
  
  return true;
}
