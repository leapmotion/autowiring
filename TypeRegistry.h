#pragma once
#include <typeinfo>

template<class T>
class JunctionBox;

class JunctionBoxBase;

namespace std {
  template<class T>
  class shared_ptr;

  template<class T, class... Args>
  std::shared_ptr<T> make_shared(Args&&...);

  template<class T, class U>
  shared_ptr<T> static_pointer_cast(const shared_ptr<U>& sp);
}

struct TypeRegistryEntry {
  TypeRegistryEntry(const std::type_info& ti, void(*factory)(std::shared_ptr<JunctionBoxBase>&));

  const TypeRegistryEntry* pFlink;
  const std::type_info& ti;
  void (*const m_NewJunctionBox)(std::shared_ptr<JunctionBoxBase>&);
};

extern const TypeRegistryEntry* g_pFirstEntry;
extern size_t g_entryCount;

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

// JunctionBox casting factory
template<class T>
void NewJunctionBox(std::shared_ptr<JunctionBoxBase>& out) {
  out = std::static_pointer_cast<JunctionBoxBase>(
    std::make_shared<JunctionBox<T>>()
  );
}

template<class T>
const TypeRegistryEntry RegType<T>::r(typeid(T), &NewJunctionBox<T>);

