#pragma once
#include <typeinfo>

struct TypeRegistryEntry {
  TypeRegistryEntry(const std::type_info& ti);

  const TypeRegistryEntry* pFlink;
  const std::type_info& ti;
};

extern const TypeRegistryEntry* g_pFirstEntry;

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
const TypeRegistryEntry RegType<T>::r(typeid(T));
