// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoPacketGraph.h"
#include "AutoPacketProfiler.h"
#include "CoreContext.h"
#include "DecorationDisposition.h"
#include "demangle.h"
#include "SatCounter.h"
#include <fstream>
#include <string>


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
      auto publisher = decoration.m_publisher;
      auto type = decoration.m_type;
      
      if (publisher && publisher->called) {
        AddEdge(type, *publisher, false);
      }
      
      for (auto& subscriber : decoration.m_subscribers) {
        if (subscriber->called) {
          AddEdge(type, *subscriber, true);
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
  
  file << "digraph \"" << autowiring::demangle(CoreContext::CurrentContext()->GetSigilType()) << " context\" {" << std::endl;
  
  std::lock_guard<std::mutex> lk(m_lock);
  
  // Containers for the unique types and descriptors
  std::unordered_set<std::string> typeNames;
  std::unordered_set<std::string> descriptorNames;
  
  for (auto& itr : m_deliveryGraph) {
    auto& edge = itr.first;
    auto type = edge.type_info;
    auto& descriptor = edge.descriptor;
    auto count = itr.second;
    
    // TODO: skip if type == AutoPacketGraph
    
    std::string typeName = autowiring::demangle(type);
    std::string descriptorName = autowiring::demangle(descriptor.GetType());
    
    // Get a unique set of types/descriptors
    if (typeNames.find(typeName) == typeNames.end())
      typeNames.insert(typeName);
    
    if (descriptorNames.find(descriptorName) == descriptorNames.end())
      descriptorNames.insert(descriptorName);
    
    // string format: "type" -> "AutoFilter" (or vice versa)
    std::stringstream ss;
    ss << "  \"";
    if (edge.input) {
      ss << typeName << "\" -> \"" << descriptorName;
    } else {
      ss << descriptorName << "\" -> \"" << typeName;
    }
    
    // TODO: count should probably be optional
    ss << "\" [label=\"" << count << "\"];" << std::endl;
    
    file << ss.str();
  }
  file << std::endl;
  
  // Setup the shapes for the types and descriptors
  for (auto& typeName : typeNames)
    file << "  \"" << typeName << "\" [shape=box];" << std::endl;
  
  for (auto& descriptorName : descriptorNames)
    file << "  \"" << descriptorName << "\" [shape=ellipse];" << std::endl;
  
  file << "}\n";
  
  return true;
}
