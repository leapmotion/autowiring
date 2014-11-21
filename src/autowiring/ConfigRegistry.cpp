// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ConfigRegistry.h"

// Head of a linked list which will have node for every event type
const ConfigRegistryEntry* g_pFirstConfigEntry = nullptr;
size_t g_configEntryCount = 0;

ConfigRegistryEntry::ConfigRegistryEntry(const std::type_info& ti) :
  pFlink(g_pFirstConfigEntry),
  ti(ti)
{
  g_configEntryCount++;
  g_pFirstConfigEntry = this;
}
