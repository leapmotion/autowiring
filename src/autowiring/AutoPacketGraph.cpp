// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
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
#include STL_UNORDERED_SET

void AutoPacketGraph::LoadEdges() {
  std::lock_guard<std::mutex> lk(m_lock);
  
  std::list<AutoFilterDescriptor> descriptors;
  m_factory->AppendAutoFiltersTo(descriptors);

  for (auto& descriptor : descriptors) {
    for(auto pCur = descriptor.GetAutoFilterArguments(); *pCur; pCur++) {
      auto_id id = pCur->id;
      
      // Skip the AutoPacketGraph
      const auto_id descType = m_factory->GetContext()->GetAutoTypeId(descriptor.GetAutoFilter());
      if (descType == auto_id_t<AutoPacketGraph>{}) {
        continue;
      }

      DeliveryEdge::ArgType arg_type;
      if (pCur->is_rvalue) {
        arg_type = DeliveryEdge::ArgType::Rvalue;
      } else if (pCur->is_input) {
        arg_type = DeliveryEdge::ArgType::Input;
      } else {
        arg_type = DeliveryEdge::ArgType::Output;
      }

      DeliveryEdge edge {id, descriptor, arg_type};
      if (m_deliveryGraph.find(edge) == m_deliveryGraph.end()) {
        m_deliveryGraph[edge] = 0;
      }
    }
  }
}

void AutoPacketGraph::RecordDelivery(auto_id id, const AutoFilterDescriptor& descriptor, DeliveryEdge::ArgType arg_type) {
  DeliveryEdge edge { id, descriptor, arg_type };

  auto itr = m_deliveryGraph.find(edge);
  assert(itr != m_deliveryGraph.end());
  itr->second++;
}

void AutoPacketGraph::NewObject(CoreContext&, const CoreObjectDescriptor&) {
  LoadEdges();
}

bool AutoPacketGraph::OnStart(void) {
  LoadEdges();
  return true;
}

void AutoPacketGraph::AutoFilter(AutoPacket& packet) {
  packet.AddTeardownListener([this, &packet] () {
    for (auto& cur : packet.GetDecorations()) {
      auto& decoration = cur.second;
      auto type = cur.first.id;

      if (decoration.m_pModifier && !decoration.m_pModifier->remaining) {
        RecordDelivery(type, *decoration.m_pModifier, DeliveryEdge::ArgType::Rvalue);
      }

      for (auto& publisher : decoration.m_publishers)
        if (!publisher->remaining)
          RecordDelivery(type, *publisher, DeliveryEdge::ArgType::Output);

      for (auto& subscriber : decoration.m_subscribers) {
        // Skip the AutoPacketGraph
        auto_id descType = m_factory->GetContext()->GetAutoTypeId(subscriber.satCounter->GetAutoFilter());
        if (descType == auto_id_t<AutoPacketGraph>{})
          continue;
        
        if (!subscriber.satCounter->remaining)
          RecordDelivery(type, *subscriber.satCounter, DeliveryEdge::ArgType::Input);
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
    auto_id descType = m_factory->GetContext()->GetAutoTypeId(descriptor.GetAutoFilter());
    if (descType == auto_id_t<AutoPacketGraph>{}) {
      continue;
    }
    
    std::string typeName = autowiring::demangle(type);
    std::string descriptorName = autowiring::demangle(descType);
    
    // Get a unique set of types/descriptors
    typeNames.insert(typeName);
    descriptorNames.insert(descriptorName);
    
    // string format: "type" -> "AutoFilter" (or vice versa)
    std::stringstream ss;
    ss << "  \"";

    if (edge.arg_type == DeliveryEdge::ArgType::Rvalue) {
      ss << typeName << "\" -> \"" << descriptorName << "\" [dir=both";
      if (numPackets)
        ss << ", label=\"" << count << "\"";
      ss << "];";
    } else {
      if (edge.arg_type == DeliveryEdge::ArgType::Input) {
        ss << typeName << "\" -> \"" << descriptorName << "\"";
      } else if (edge.arg_type == DeliveryEdge::ArgType::Output) {
        ss << descriptorName << "\" -> \"" << typeName << "\"";
      }
      if (numPackets)
        ss << " [label=\"" << count << "\"];";
    }
    
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
