#pragma once
#include <typeinfo>
#include "JunctionBox.h"
#include FUNCTIONAL_HEADER
#include SHARED_PTR_HEADER

class JunctionBoxBase;

struct TypeRegistryEntry {
  TypeRegistryEntry(const std::type_info& ti, std::function<std::shared_ptr<JunctionBoxBase>(void)> factory);

  const TypeRegistryEntry* pFlink;
  const std::type_info& ti;
  std::function<std::shared_ptr<JunctionBoxBase>(void)> m_NewJunctionBox;
};

extern const TypeRegistryEntry* g_pFirstEntry;
extern size_t g_entryCount;

///JunctionBox factory
template<class T>
std::shared_ptr<JunctionBox<T>> NewJunctionBox(){
  return std::make_shared<JunctionBox<T>>();
}

/// <summary>
/// Adds the specified type to the universal type registry
/// </summary>
/// <remarks>
/// Any instance of this type registry parameterized on type T will be added to the
/// global static type registry, and this registry is computed at link time.
/// </remarks>
template<class T>
class RegType
{
public:
  static const TypeRegistryEntry r;
};

template<class T>
const TypeRegistryEntry RegType<T>::r(typeid(T), &NewJunctionBox<T>);

