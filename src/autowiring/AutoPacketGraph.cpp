// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoPacketGraph.h"
#include "CoreContext.h"
#include "DecorationDisposition.h"
#include "demangle.h"
#include "SatCounter.h"
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include FUNCTIONAL_HEADER

AutoPacketGraph::AutoPacketGraph() {
}

std::string AutoPacketGraph::DemangleTypeName(const std::type_info* type_info) const {
  std::string demangled = autowiring::demangle(type_info);
  
  size_t demangledLength = demangled.length();
  size_t newLength =
    demangled[demangledLength - 2] == ' ' ?
    demangledLength - 10 :
    demangledLength - 9;
  
  return demangled.substr(demangled.find("<") + 1, newLength);
}

void AutoPacketGraph::LoadEdges() {
  std::lock_guard<std::mutex> lk(m_lock);
  
  std::list<AutoFilterDescriptor> descriptors;
  m_factory->AppendAutoFiltersTo(descriptors);
  
  for (auto& descriptor : descriptors) {
    for(auto pCur = descriptor.GetAutoFilterInput(); *pCur; pCur++) {
      const std::type_info& type_info = *pCur->ti;
      
      // Skip the AutoPacketGraph
      const std::type_info& descType = m_factory->GetContext()->GetAutoTypeId(descriptor.GetAutoFilter());
      if (descType == typeid(AutoPacketGraph)) {
        continue;
      }
      
      if (pCur->is_input) {
        DeliveryEdge edge { &type_info, descriptor, true };
        if (m_deliveryGraph.find(edge) == m_deliveryGraph.end()) {
          m_deliveryGraph[edge] = 0;
        }
      }
      
      if (pCur->is_output) {
        DeliveryEdge edge { &type_info, descriptor, false };
        if (m_deliveryGraph.find(edge) == m_deliveryGraph.end()) {
          m_deliveryGraph[edge] = 0;
        }
      }
    }
  }
}

void AutoPacketGraph::RecordDelivery(const std::type_info* ti, const AutoFilterDescriptor& descriptor, bool input) {
  DeliveryEdge edge { ti, descriptor, input };
  
  auto itr = m_deliveryGraph.find(edge);
  assert(itr != m_deliveryGraph.end());
  itr->second++;
}

void AutoPacketGraph::NewObject(CoreContext&, const ObjectTraits&) {
  LoadEdges();
}

bool AutoPacketGraph::OnStart(void) {
  LoadEdges();
  
  return false;
}

void AutoPacketGraph::AutoFilter(AutoPacket& packet) {
  packet.AddTeardownListener([this, &packet] () {
    for (auto& decoration : packet.GetDispositions()) {
      auto type = &decoration.GetKey().ti;

      for (auto& publisher : decoration.m_publishers) {
        if (publisher->called) {
          RecordDelivery(type, *publisher, false);
        }
      }

      for (auto& subscriber : decoration.m_subscribers) {
        // Skip the AutoPacketGraph
        const std::type_info& descType = m_factory->GetContext()->GetAutoTypeId(subscriber->GetAutoFilter());
        if (descType == typeid(AutoPacketGraph)) {
          continue;
        }
        
        if (subscriber->called) {
          RecordDelivery(type, *subscriber, true);
        }
      }
    }
  });
}

bool AutoPacketGraph::WriteGV(const std::string& filename, bool numPackets) const {
  std::ofstream file(filename.c_str());
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
    
    // Skip the AutoPacketGraph
    const std::type_info& descType = m_factory->GetContext()->GetAutoTypeId(descriptor.GetAutoFilter());
    if (descType == typeid(AutoPacketGraph)) {
      continue;
    }
    
    std::string typeName = DemangleTypeName(type);
    std::string descriptorName = autowiring::demangle(descType);
    
    // Get a unique set of types/descriptors
    if (typeNames.find(typeName) == typeNames.end())
      typeNames.insert(typeName);
    
    if (descriptorNames.find(descriptorName) == descriptorNames.end())
      descriptorNames.insert(descriptorName);
    
    // string format: "type" -> "AutoFilter" (or vice versa)
    std::stringstream ss;
    ss << "  \"";
    if (edge.input) {
      ss << typeName << "\" -> \"" << descriptorName << "\"";
    } else {
      ss << descriptorName << "\" -> \"" << typeName << "\"";
    }
    
    if (numPackets)
      ss << "[label=\"" << count << "\"];";
    
    ss << std::endl;
    
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
