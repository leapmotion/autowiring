// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include MEMORY_HEADER
#include <string>
#include <sstream>
#include "AnySharedPointer.h"

struct ConfigRegistryEntry {
  ConfigRegistryEntry(const std::type_info& ti);

  // Next entry in the list:
  const ConfigRegistryEntry* const pFlink;
  
  // Configuration name
  const std::string m_key;
  
  bool is(const std::string& key) const;
  
  virtual AnySharedPointer parse(const std::string&) const = 0;
};

template<class T, class Key>
struct ConfigRegistryEntryT:
  public ConfigRegistryEntry
{
  ConfigRegistryEntryT(void):
    ConfigRegistryEntry(typeid(Key))
  {}
  
  AnySharedPointer parse(const std::string& str) const {
    std::istringstream ss(str);
    T val;
    ss >> val;
    return AnySharedPointer(std::make_shared<T>(val));
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
template<class T, class Key>
class RegConfig
{
public:
  static const ConfigRegistryEntryT<T, Key> r;
};

template<class T, class Key>
const ConfigRegistryEntryT<T, Key> RegConfig<T, Key>::r;
