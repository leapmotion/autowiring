// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "EventRegistry.h"

// Head of a linked list which will have node for every event type
const EventRegistryEntry* g_pFirstEventEntry = nullptr;
size_t g_eventEntryCount = 0;

EventRegistryEntry::EventRegistryEntry(const std::type_info& ti) :
  pFlink(g_pFirstEventEntry),
  ti(ti)
{
  g_eventEntryCount++;
  g_pFirstEventEntry = this;
}
