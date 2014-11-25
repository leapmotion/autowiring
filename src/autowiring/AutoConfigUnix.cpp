// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoConfig.h"
#include "demangle.h"
#include <regex>
#include <sstream>
#include <iostream>

static const std::regex namePattern(".*ConfigTypeExtractor<(?:class |struct )?(\\w*)(?:, (?:class |struct )?(\\w*))>$");

static std::string FormatKey(const std::smatch& match) {
  // If no namespace, just return match
  if (match.size() == 2) {
    return match.str(1);
  }
  
  std::stringstream ss;
  ss << match.str(1) << "." << match.str(2);
  return ss.str();
}

static std::string ExtractKey(const std::type_info& ti) {
  std::smatch sm;
  std::cout << autowiring::demangle(ti) << std::endl;
  std::regex_match(autowiring::demangle(ti), sm, namePattern);
  
  assert(sm.size() == 2 || sm.size() == 3 && "Regex couldn't find type name");
  
  return FormatKey(sm);
}

AutoConfigBase::AutoConfigBase(const std::type_info& ti):
  m_key(ExtractKey(ti))
{}
