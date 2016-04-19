// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "config_descriptor.h"
#include <atomic>

namespace autowiring {
  struct config_registry_entry_base {
  protected:
    config_registry_entry_base(void);

  public:
    // Next entry in the registry:
    config_registry_entry_base* pFlink = nullptr;
  };

  struct config_registry_entry_default {
    static const config_descriptor desc;
  };

  template<typename T, typename = void>
  struct config_registry_entry :
    config_registry_entry_default
  {};

  template<typename T>
  struct config_registry_entry<T, typename std::enable_if<has_getconfigdescriptor<T>::value>::type> :
    config_registry_entry_base
  {
  public:
    static const config_descriptor desc;
  };

  template<typename T>
  const config_descriptor config_registry_entry<T, typename std::enable_if<has_getconfigdescriptor<T>::value>::type>::desc = T::GetConfigDescriptor();

  extern std::atomic<config_registry_entry_base*> g_pFirstEntry;

  /// <summary>
  /// Gets a string representation of the named configuration value
  /// </summary>
  template<typename T>
  std::string ConfigGet(const char* name, T& obj) {
    const config_descriptor& desc = config_registry_entry<T>::desc;
    auto q = desc.fields.find(name);
    if (q == desc.fields.end())
      throw std::invalid_argument("Configuration name not found in the specified object's configuration descriptor");

    const autowiring::config_field& f = q->second;
    return f.marshaller->marshal(reinterpret_cast<uint8_t*>(&obj) + f.offset);
  }

  /// <summary>
  /// Sets the named configuration value to the specified string value
  /// </summary>
  template<typename T>
  void ConfigSet(const char* name, T& obj, const char* value) {
    const config_descriptor& desc = config_registry_entry<T>::desc;
    auto q = desc.fields.find(name);
    if (q == desc.fields.end())
      throw std::invalid_argument("Configuration name not found in the specified object's configuration descriptor");

    const autowiring::config_field& f = q->second;
    f.marshaller->unmarshal(
      reinterpret_cast<uint8_t*>(&obj) + f.offset,
      value
    );
  }
}
