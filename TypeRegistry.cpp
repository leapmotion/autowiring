#include "stdafx.h"
#include "TypeRegistry.h"

const TypeRegistryEntry* g_pFirstEntry = nullptr;
size_t g_entryCount = 0;

TypeRegistryEntry::TypeRegistryEntry(const std::type_info& ti) :
  pFlink(g_pFirstEntry),
  ti(ti)
{
  g_entryCount++;
  g_pFirstEntry = this;
}