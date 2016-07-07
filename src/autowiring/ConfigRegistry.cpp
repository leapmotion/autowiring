// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ConfigRegistry.h"

using namespace autowiring;

std::atomic<config_registry_entry_base*> autowiring::config_registry::g_pFirstEntry;
const config_registry_entry_default config_registry_entry_default::entry = {};

config_registry_entry_default::config_registry_entry_default(void) = default;

config_registry_entry_base::config_registry_entry_base(config_descriptor&& descriptor) :
  descriptor(std::move(descriptor)),
  pFlink(config_registry::Link(*this))
{
}

const config_registry_entry_base* config_registry::Link(config_registry_entry_base& entry) {
  config_registry_entry_base* pFlink = g_pFirstEntry;
  while (!g_pFirstEntry.compare_exchange_weak(pFlink, &entry));
  return pFlink;
}
