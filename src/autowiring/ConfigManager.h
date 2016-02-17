// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "config_descriptor.h"
#include "optional.h"
#include <string>
#include <unordered_map>

namespace autowiring {
  class ConfigManager {
  public:
    ConfigManager(void);

  private:
    spin_lock m_lock;

    // A single entry, which has a string representation part paired
    // with a pointer to the value part
    struct Entry {
      // The value held here
      optional<std::string> value;

      struct Attachment {
        const config_field* field;
        void* pObj;
      };

      // Fields attached on this entry:
      std::vector<Attachment> attached;
    };

    // Empty string sentry, used when a miss occurs
    const std::string m_empty;

    // All configuration values
    std::unordered_map<std::string, Entry> m_config;

  public:
    /// <summary>
    /// Registers a new configurable object
    /// </summary>
    /// <param name="pObj">The object that may be configured</param>
    /// <param name="desc">A descriptor for the object</param>
    void Register(void* pObj, const config_descriptor& desc);

    /// <summary>
    /// Gets the current configuration value from the map
    /// </summary>
    const std::string& Get(std::string name) const;

    /// <summary>
    /// Sets the named config value in the map
    /// </summary>
    void Set(std::string&& name, std::string&& value);
  };
}
