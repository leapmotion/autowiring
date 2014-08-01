// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "CreationRules.h"
#include "CoreContext.h"
#include <typeinfo>
#include STL_TUPLE_HEADER
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
  TypeRegistryEntry(const std::type_info& ti);

  // Next entry in the list:
  const TypeRegistryEntry* const pFlink;

  // Type of this entry:
  const std::type_info& ti;

  /// <summary>
  /// The runtime type information corresponding to this entry
  /// </summary>
  virtual const std::type_info& GetTypeInfo(void) const = 0;

  /// <returns>
  /// True if this type is an event receiver type
  /// </returns>
  //virtual bool IsEventReceiver(void) const = 0;

  /// <summary>
  /// Constructor method, used to generate a new junction box
  /// </summary>
  virtual std::shared_ptr<JunctionBoxBase> NewJunctionBox(void) const = 0;

  /// <summary>
  /// Used to create a type identifier value, for use with AutoNet
  /// </summary>
  virtual std::shared_ptr<TypeIdentifierBase> NewTypeIdentifier(void) const = 0;

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

  //bool IsEventReceiver(void) const override { return std::is_base_of<EventReceiver, T>::value; }

  virtual std::shared_ptr<JunctionBoxBase> NewJunctionBox(void) const override {
    return std::static_pointer_cast<JunctionBoxBase>(
      std::make_shared<JunctionBox<T>>()
    );
  }

  std::shared_ptr<TypeIdentifierBase> NewTypeIdentifier(void) const override {
    return std::static_pointer_cast<TypeIdentifierBase>(
      std::make_shared<TypeIdentifier<T>>()
    );
  }

  bool CanInject(void) const override {
    return is_injectable<T>::value;
  }

  template<typename U>
  typename std::enable_if<is_injectable<U>::value>::type AnyInject(void) const {
    CoreContext::CurrentContext()->Inject<U>();
  }

  template<typename U>
  typename std::enable_if<!is_injectable<U>::value>::type AnyInject(void) const {
    throw autowiring_error("Attempted to inject a non-injectable type");
  }

  virtual void Inject(void) const override { AnyInject<T>(); }
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
  static const TypeRegistryEntryT<T> r;
};

template<class T>
const TypeRegistryEntryT<T> RegType<T>::r;
