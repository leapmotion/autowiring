#pragma once
#include <typeinfo>
#include MEMORY_HEADER

template<class T>
class JunctionBox;

class JunctionBoxBase;
class Object;

// Checks if an Object* listens to a event T;
struct TypeIdentifierBase {
  virtual bool Is(const Object* obj) = 0;
  virtual const std::type_info& Type() = 0;
};

template<typename T>
struct TypeIdentifier:
  public TypeIdentifierBase
{
  // true if "obj" is an event receiver for T
  bool Is(const Object* obj){
    return !!dynamic_cast<const T*>(obj);
  }
  
  const std::type_info& Type(){
    return typeid(T);
  }
};

struct TypeRegistryEntry {
  TypeRegistryEntry(const std::type_info& ti, void(*factory)(std::shared_ptr<JunctionBoxBase>&),
                                              std::shared_ptr<TypeIdentifierBase>(*identifier)(void));

  const TypeRegistryEntry* pFlink;
  const std::type_info& ti;
  void (*const m_NewJunctionBox)(std::shared_ptr<JunctionBoxBase>&);
  std::shared_ptr<TypeIdentifierBase>(*const m_NewTypeIdentifier)(void);
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

// TypeIdentifier casting factory
template<class T>
std::shared_ptr<TypeIdentifierBase> NewTypeIdentifier(void) {
  return std::static_pointer_cast<TypeIdentifierBase>(
    std::make_shared<TypeIdentifier<T>>()
  );
}

template<class T>
const TypeRegistryEntry RegType<T>::r(typeid(T), &NewJunctionBox<T>, &NewTypeIdentifier<T>);
