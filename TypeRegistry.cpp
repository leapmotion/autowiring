#include "stdafx.h"
#include "TypeRegistry.h"

const TypeRegistryEntry* g_pFirstEntry = nullptr;

TypeRegistryEntry::TypeRegistryEntry(const std::type_info& ti) :
  pFlink(g_pFirstEntry),
  ti(ti)
{
  g_pFirstEntry = this;
}