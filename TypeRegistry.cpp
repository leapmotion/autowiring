#include "stdafx.h"
#include "TypeRegistry.h"
#include "JunctionBox.h"

// Head of a linked list which will have node for every event type
const TypeRegistryEntry* g_pFirstEntry = nullptr;
size_t g_entryCount = 0;

TypeRegistryEntry::TypeRegistryEntry(const std::type_info& ti, void(*factory)(std::shared_ptr<JunctionBoxBase>&),
                                                               std::shared_ptr<TypeIdentifierBase>(*identifier)(void) ):
  ti(ti),
  pFlink(g_pFirstEntry),
  m_NewJunctionBox(factory),
  m_NewTypeIdentifier(identifier)
{
  g_entryCount++;
  g_pFirstEntry = this;
}