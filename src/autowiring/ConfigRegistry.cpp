// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ConfigRegistry.h"
#include "demangle.h"
#include <regex>
#include <sstream>

static const std::regex namePattern(".*ConfigTypeExtractor<(?:class |struct )?(\\w*)>");

static std::string ExtractFieldName(const std::type_info& ti) {
  std::smatch sm;
  std::regex_match(autowiring::demangle(ti), sm, namePattern);
  return sm.str(1);
}

// Head of a linked list which will have node for every event type
const ConfigRegistryEntry* g_pFirstConfigEntry = nullptr;
size_t g_configEntryCount = 0;

ConfigRegistryEntry::ConfigRegistryEntry(const std::type_info& tinfo) :
  pFlink(g_pFirstConfigEntry),
  ti(tinfo),
  name(ExtractFieldName(tinfo))
{
  g_configEntryCount++;
  g_pFirstConfigEntry = this;
}

bool ConfigRegistryEntry::validName(const std::type_info& tinfo) const {
  return name == ExtractFieldName(tinfo);
}
