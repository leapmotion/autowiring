// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoConfig.h"
#include "demangle.h"
#include <regex>
#include <sstream>
#include <iostream>

static const std::regex namePattern(".*ConfigTypeExtractor<(?:class |struct )?(\\w*)(?:, (?:class |struct )?(\\w*))?>$");
static const int NAMESPACE_INDEX = 1;
static const int FIELD_INDEX = 2;

static std::string FormatKey(const std::smatch& match) {
  // If no namespace, then field will be empty
  if (match.str(FIELD_INDEX).empty()) {
    return match.str(NAMESPACE_INDEX);
  }
  
  std::stringstream ss;
  ss << match.str(NAMESPACE_INDEX) << "." << match.str(FIELD_INDEX);
  std::cout << "KEY: " << ss.str() << std::endl;
  return ss.str();
}

static std::string ExtractKey(const std::type_info& ti) {
  std::smatch sm;
  std::regex_match(autowiring::demangle(ti), sm, namePattern);
  
  assert(sm.size() == 3 && "Regex couldn't properly parse type name");
  
  return FormatKey(sm);
}

AutoConfigBase::AutoConfigBase(const std::type_info& ti):
  m_key(ExtractKey(ti))
{}
