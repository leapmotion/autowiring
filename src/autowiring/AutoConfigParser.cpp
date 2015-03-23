// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoConfigParser.hpp"
#include "demangle.h"
#include <stdexcept>

std::string autowiring::ExtractKey(const std::string& demangled) {
  //Extract Namespace and value from typename
  //struct ConfigTypeExtractor<struct Namespace, class ..., struct Value>
  //Or on unix: ConfigTypeExtractor<Namespace, ..., Value>

  const auto identifiersStart = demangled.find('<');
  const auto identifiersEnd = demangled.rfind('>');
  if (identifiersStart == (std::string::npos) || identifiersEnd == std::string::npos)
    return std::string();

  std::string key;
  
  auto subKeyStart = identifiersStart;
  while (subKeyStart < identifiersEnd) {
    //Find the > or , which denotes the start of the next identifier (or the end of the identifiers)
    auto subKeyEnd = demangled.find_first_of(",>", subKeyStart);

    //Find the end of :: or the space between the struct declaration and our name backwards from the end of the identifer
    
    auto subKeyStartTrimmed = demangled.find_last_of(" <,", subKeyEnd-1);
    if(subKeyStartTrimmed != std::string::npos && subKeyStartTrimmed > subKeyStart)
      subKeyStart = subKeyStartTrimmed;

    ++subKeyStart; //skip the space, :, or < that comes at the start
    key += demangled.substr(subKeyStart, (subKeyEnd-subKeyStart));
    key += ".";

    subKeyStart = subKeyEnd + 1;
  }

  key.resize(key.size() - 1); //trim the trailing .
  return key;
}

std::string autowiring::ExtractKey(const std::type_info& ti) {
  bool success;
  std::string demangled = demangle(ti,&success);
  
  if(! success )
    throw std::runtime_error("Demangle failed, structure may not be used as part of a key. Is your struct declared inside a function? Symbol=" + demangled);
  
  return ExtractKey(demangled);
}
