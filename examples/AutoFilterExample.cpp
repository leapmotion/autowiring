// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include <autowiring/autowiring.h>
#include <iostream>
#include <string>

// 
// AutoFilter
// 
// AutoFilter provides a declarative way to to create a filter graph network
// within a context. Packets on this network have the same domain as events
// (snooping works with packets as well). An AutoFilter is made by creating 
// a ContextMember that implements the AutoFilter(...) member function.
//  

class StringFilter {
public:
  void AutoFilter(std::string msg) {
    std::cout << "Filter Update: " << msg << std::endl;
  }
};

class StringIntFilter {
public:
  void AutoFilter(std::string msg, int num) {
    std::cout << "Other Filter Update: " << msg << " : " << num << std::endl;
  }
};

int main() {
  // Initiate the current context
  AutoCurrentContext()->Initiate();
  
  // Inject our AutoFilter enabled context members into the context
  AutoRequired<StringFilter>();
  AutoRequired<StringIntFilter>();
  
  // Each context automatically includes an AutoPacketFactory type. This
  // can be used to create new packets in the AutoFilter network
  Autowired<AutoPacketFactory> factory;
  
  // When decorating a packet, all AutoFilters that have that type as an argument
  // will be called. It is only called when all argument types have been decorated
  auto packet = factory->NewPacket();
  
  packet->Decorate(std::string("Hello World"));
  
  std::cout << "StringIntFilter not called yet" << std::endl;
  
  // StringIntFilter will now be called since all AutoFilter arguments have ben decorated
  packet->Decorate(42);
}
