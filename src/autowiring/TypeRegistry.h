// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include "autowiring_error.h"
#include "CreationRules.h"
#include "TypeIdentifier.h"
#include MEMORY_HEADER

namespace autowiring {

template<typename T>
void InjectCurrent(void);

struct TypeRegistryEntry:
  public TypeIdentifierBase
{
  TypeRegistryEntry(const std::type_info& ti);

  // Next entry in the list:
  const TypeRegistryEntry* const pFlink;

  // Type of this entry:
  const std::type_info& ti;

  /// <summary>
  /// Returns true if an injection is possible on the described type
  /// </summary>
  /// <remarks>
  /// Injection might not be possible if the named type does not have a zero-argument constructor.
  /// In this case, an exception will be thrown if an attempt is made to call Inject
  /// </remarks>
  virtual bool CanInject(void) const = 0;

  /// <summary>
  /// Injects the type described by this registry entry in the current context
  /// </summary>
  /// <remarks>
  /// If the underlying type cannot be injected, this method will throw an exception
  /// </remarks>
  virtual void Inject(void) const = 0;
};

template<class T>
struct TypeRegistryEntryT:
  public TypeRegistryEntry
{
  TypeRegistryEntryT(void):
    TypeRegistryEntry(typeid(T))
  {}

  virtual const std::type_info& GetTypeInfo(void) const override { return typeid(T); }

  // true if "obj" is an event receiver for T
  bool IsSameAs(const CoreObject* obj) const override {
    return !!dynamic_cast<const T*>(obj);
  }

  bool CanInject(void) const override {
    return is_injectable<T>::value;
  }

  template<typename U>
  typename std::enable_if<is_injectable<U>::value>::type AnyInject(void) const {
    autowiring::InjectCurrent<U>();
  }

  template<typename U>
  typename std::enable_if<!is_injectable<U>::value>::type AnyInject(void) const {
    throw autowiring_error("Attempted to inject a non-injectable type");
  }

  virtual void Inject(void) const override { AnyInject<T>(); }
};

extern const TypeRegistryEntry* g_pFirstTypeEntry;
extern size_t g_typeEntryCount;

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
  static const TypeRegistryEntryT<T> r;
};

template<class T>
const TypeRegistryEntryT<T> RegType<T>::r;
}
