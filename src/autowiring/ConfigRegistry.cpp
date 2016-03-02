// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ConfigRegistry.h"

using namespace autowiring;

std::atomic<config_registry_entry_base*> autowiring::g_pFirstEntry;

config_registry_entry_base::config_registry_entry_base(void) {
  while (!g_pFirstEntry.compare_exchange_weak(pFlink, this));
}
