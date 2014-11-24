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

  // Type of this entry:
  const std::type_info& ti;
  
  // Configuration name
  const std::string name;
  
  bool validName(const std::type_info& ti) const;
  
  virtual AnySharedPointer& parse(const std::string&) const = 0;
};

template<class T, class NAME>
struct ConfigRegistryEntryT:
  public ConfigRegistryEntry
{
  ConfigRegistryEntryT(void):
    ConfigRegistryEntry(typeid(NAME))
  {}
  
  AnySharedPointer& parse(const std::string& str) const {
    std::istringstream ss(str);
    T val;
    ss >> val;
    AnySharedPointer retval(std::make_shared<T>(val));
    return retval;
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
template<class T, class NAME>
class RegConfig
{
public:
  static const ConfigRegistryEntryT<T, NAME> r;
};

template<class T, class NAME>
const ConfigRegistryEntryT<T, NAME> RegConfig<T, NAME>::r;
