// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "ConfigRegistry.h"
#include "autowiring_error.h"
#include <string>
#include <sstream>
#include STL_UNORDERED_MAP
#include STL_UNORDERED_SET
#include MEMORY_HEADER

struct AnySharedPointer;

class AutoConfigManager:
  public ContextMember
{
public:
  AutoConfigManager();
  virtual ~AutoConfigManager();
  
  // Callback function type
  typedef std::function<void(const AnySharedPointer&)> t_callback;
  
private:
  std::mutex m_lock;
  
  // local map of the Config registry
  const std::unordered_map<std::string, const ConfigRegistryEntry*> m_registry;
  
  // Values of AutoConfigs in this context
  std::unordered_map<std::string, AnySharedPointer> m_attributes;
  
  // Set of keys for values set from this context
  std::unordered_set<std::string> m_setHere;
  
  // map of callbacks registered for a key
  std::unordered_map<std::string, std::vector<t_callback>> m_callbacks;

public:
  /// <summary>
  /// Check if this key has been set
  /// </summary>
  bool IsConfigured(const std::string& key);
  
  /// <summary>
  /// Get a reference to where the config value is stored
  /// </summary>
  /// <remarks>
  /// This method will throw an exception if the specified name cannot be found as a configurable value
  /// in the application, or if the specified value type does not match the type expected by this field
  /// </remarks>
  AnySharedPointer& Get(const std::string& key);
  
  /// <summary>
  /// Assigns the specified value to an AnySharedPointer slot
  /// </summary>
  /// <remarks>
  /// This method will throw an exception if the specified name cannot be found as a configurable value
  /// in the application, or if the specified value type does not match the type expected by this field
  /// </remarks>
  template<class T>
  void Set(const std::string& key, const T& value) {
    std::lock_guard<std::mutex> lk(m_lock);
    
    if (!m_registry.count(key)) {
      std::stringstream ss;
      ss << "No configuration found for key '" << key << "'";
      throw autowiring_error(ss.str());
    }
    
    if (!m_registry.at(key)->verifyType(typeid(T))) {
      std::stringstream ss;
      ss << "Attempting to set config '" << key << "' with incorrect type '"
         << autowiring::demangle(typeid(T)) << "'";
      throw autowiring_error(ss.str());
    }
    
    // Set value in this AutoConfigManager
    SetInternal(key, AnySharedPointer(std::make_shared<T>(value)));
  }
  
  /// <summary>
  /// Overload for c-style string. Converts to std::string
  /// </summary>
  void Set(const std::string& key, const char* value);

  /// <summary>
  /// Coerces the string representation of the specified field to the correct value type
  /// </summary>
  /// <remarks>
  /// This method will throw an exception if there is no string converter available on this type
  /// </remarks>
  /// <returns>
  /// True if value successfully set, False if key not found.
  /// </return>
  bool SetParsed(const std::string& key, const std::string& value);
  
  // Add a callback for when key is changed
  void AddCallback(const std::string& key, std::function<void(const AnySharedPointer&)>&& fx);
  
private:
  // Handles setting a value that has already been parsed into an AnySharedPointer
  // Must hold m_lock when calling this
  void SetInternal(const std::string& key, AnySharedPointer value);
};
