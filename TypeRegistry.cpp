#include "stdafx.h"
#include "TypeRegistry.h"

const TypeRegistryEntry* g_pFirstEntry = nullptr;
size_t g_entryCount = 0;

TypeRegistryEntry::TypeRegistryEntry(const std::type_info& ti, std::function<JunctionBoxBase*(void)> factory) :
  ti(ti),
  pFlink(g_pFirstEntry),
  m_pFactory(factory)
{
  g_entryCount++;
  g_pFirstEntry = this;
}