// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "TypeIdentifier.h"
#include MEMORY_HEADER

struct ConfigRegistryEntry {
  ConfigRegistryEntry(const std::type_info& ti);

  // Next entry in the list:
  const ConfigRegistryEntry* const pFlink;

  // Type of this entry:
  const std::type_info& ti;

  /// <summary>
  /// The runtime type information corresponding to this entry
  /// </summary>
  virtual const std::type_info& GetTypeInfo(void) const = 0;

  /// <summary>
  /// Used to create a type identifier value, for use with AutoNet
  /// </summary>
  virtual std::shared_ptr<TypeIdentifierBase> NewTypeIdentifier(void) const = 0;
};

template<class T>
struct ConfigRegistryEntryT:
  public ConfigRegistryEntry
{
  ConfigRegistryEntryT(void):
    ConfigRegistryEntry(typeid(T))
  {}

  virtual const std::type_info& GetTypeInfo(void) const override { return typeid(T); }

  std::shared_ptr<TypeIdentifierBase> NewTypeIdentifier(void) const override {
    return std::static_pointer_cast<TypeIdentifierBase>(
      std::make_shared<TypeIdentifier<T>>()
    );
  }
};

extern const ConfigRegistryEntry* g_pFirstConfigEntry;
extern size_t g_confgiEntryCount;

/// <summary>
/// Adds the specified type to the universal type registry
/// </summary>
/// <remarks>
/// Any instance of this type registry parameterized on type T will be added to the
/// global static type registry, and this registry is computed at link time.
/// </remarks>
template<class T>
class RegConfig
{
public:
  static const ConfigRegistryEntryT<T> r;
};

template<class T>
const ConfigRegistryEntryT<T> RegConfig<T>::r;
