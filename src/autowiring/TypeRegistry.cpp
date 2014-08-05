// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TypeRegistry.h"
#include "JunctionBox.h"

// Head of a linked list which will have node for every event type
const TypeRegistryEntry* g_pFirstTypeEntry = nullptr;
size_t g_typeEntryCount = 0;

TypeRegistryEntry::TypeRegistryEntry(const std::type_info& ti) :
  pFlink(g_pFirstTypeEntry),
  ti(ti)
{
  g_typeEntryCount++;
  g_pFirstTypeEntry = this;
}
