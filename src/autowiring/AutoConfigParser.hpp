// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "demangle.h"
#include <regex>
#include <sstream>
#include <iostream>

namespace autowiring {

static std::string FormatKey(const std::smatch& match) {
  // Indicies into the regex match
  static const int NAMESPACE_INDEX = 1;
  static const int FIELD_INDEX = 2;
  
  // If no namespace, then field will be empty
  if (match.str(FIELD_INDEX).empty()) {
    return match.str(NAMESPACE_INDEX);
  }
  
  std::stringstream ss;
  ss << match.str(NAMESPACE_INDEX) << "." << match.str(FIELD_INDEX);
  return ss.str();
}

static std::string ExtractKey(const std::type_info& ti) {
  // Regex pattern
  static const std::regex NamePattern(
    ".*ConfigTypeExtractor<(?:class |struct )?(\\w*)(?:, (?:class |struct )?(\\w*))?>$"
  );
  
  std::smatch sm;
  std::regex_match(demangle(ti), sm, NamePattern);
  
  return FormatKey(sm);
}
  
}//namespace autowiring
