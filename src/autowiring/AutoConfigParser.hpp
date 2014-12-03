// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "demangle.h"
#include "expect.hpp"
#include <sstream>
#include <iostream>
#include <cstring>

namespace autowiring {

static std::string ExtractKeyUnix(std::stringstream& ss) {
  //Extract Namespace and value from typename
  //AutoConfigBase::ConfigTypeExtractor<Namespace, Value>
  
  std::string arg1;
  std::string arg2;
  ss >> expect("AutoConfigBase::ConfigTypeExtractor<");
  ss >> arg1;
  
  // If arg1 contains a comma, there are 2 arguments
  auto found = arg1.find(",");
  if (found != std::string::npos) {
    ss >> arg2;
    
    // Remove trailing ","
    arg1.pop_back();
    
    // Remove trailing '>'
    arg2.pop_back();
    
    std::stringstream key;
    key << arg1 << "." << arg2;
    return key.str();
    
  } else {
    // Remove trailing '>'
    arg1.pop_back();
    return arg1;
  }
}

static std::string ExtractKeyWin(std::stringstream& ss) {
  //Extract Namespace and value from typename
  //struct AutoConfigBase::ConfigTypeExtractor<struct Namespace, struct Value>
  
  std::string arg1;
  std::string arg2;
  ss >> expect("struct AutoConfigBase::ConfigTypeExtractor<struct");
  ss >> arg1;
  
  // If arg1 contains a comma, there are 2 arguments
  auto found = arg1.find(",struct");
  if (found != std::string::npos) {
    ss >> arg2;
    
    // Remove trailing ",struct"
    arg1 = arg1.substr(0, found);
    
    // Remove trailing '>'
    arg2.pop_back();
    
    std::stringstream key;
    key << arg1 << "." << arg2;
    return key.str();
    
  } else {
    // Remove trailing '>'
    arg1.pop_back();
    return arg1;
  }
}

static std::string ExtractKey(const std::type_info& ti) {
  std::string demangled = demangle(ti);
  std::stringstream ss(demangled);

  return
#if __GNUG__// Mac and linux
    ExtractKeyUnix(ss);
#else // Windows
    ExtractKeyWin(ss);
#endif
}

}//namespace autowiring
