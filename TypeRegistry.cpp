#include "stdafx.h"
#include "TypeRegistry.h"

// Head of a linked list which will have node for every event type
const TypeRegistryEntry* g_pFirstEntry = nullptr;
size_t g_entryCount = 0;

TypeRegistryEntry::TypeRegistryEntry(const std::type_info& ti, std::function<std::shared_ptr<JunctionBoxBase>(void)> factory) :
  ti(ti),
  pFlink(g_pFirstEntry),
  m_NewJunctionBox(factory)
{
  g_entryCount++;
  g_pFirstEntry = this;
}