// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ConfigRegistry.h"
#include "AutoConfigParser.hpp"
#include "demangle.h"

// Head of a linked list which will have node for every event type
const ConfigRegistryEntry* g_pFirstConfigEntry = nullptr;
size_t g_configEntryCount = 0;

ConfigRegistryEntry::ConfigRegistryEntry(const std::type_info& tinfo, bool has_validator) :
  pFlink(g_pFirstConfigEntry),
  m_key(autowiring::ExtractKey(tinfo)),
  m_has_validator(has_validator)
{
  g_configEntryCount++;
  g_pFirstConfigEntry = this;
}

bool ConfigRegistryEntry::is(const std::string& key) const {
  return m_key == key;
}

void autowiring::ThrowFailedTypeParseException(const std::string& str, const std::type_info& ti) {
  std::stringstream msg;
  msg << "Failed to parse '" << str << "' as type '" << autowiring::demangle(ti, nullptr) << "'";
  throw autowiring_error(msg.str());
}
