// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "autowiring_error.h"
#include "ConfigRegistry.h"
#include "ContextMember.h"
#include <string>
#include <vector>
#include STL_UNORDERED_MAP
#include STL_UNORDERED_SET
#include MUTEX_HEADER
#include MEMORY_HEADER

class AutoConfigVarBase;

class AutoConfigListing:
  public ContextMember
{
public:
  AutoConfigListing();
  virtual ~AutoConfigListing();
  
  // Callback signal type
  typedef autowiring::signal<void(const AutoConfigVarBase&)> onAddSignal_t;

  // Validator function type
  typedef std::function<bool(const AnySharedPointer&)> t_validator;
  
private:
  // Local map of the config registry
  static const std::unordered_map<std::string, const ConfigRegistryEntry*> s_registry;
  
  // Validators for keys that have them.
  static const std::unordered_map<std::string, t_validator> s_validators;
  
  // Lock for all members
  std::mutex m_lock;
  
  // Map of AutoConfigs in this context
  std::unordered_map<std::string, std::weak_ptr<AutoConfigVarBase>> m_values;
  
  // list of keys set locally from this context in order of creation.
  std::vector<std::string> m_orderedKeys;

  // Exception throwers:
  void ThrowKeyNotFoundException(const std::string& key) const;
  void ThrowTypeMismatchException(const std::string& key, const std::type_info& ti) const;

public:
  /// <summary>
  /// Check if this key has been set
  /// </summary>
  bool IsConfigured(const std::string& key);
  
  /// <summary>
  /// Check if this key was inherited from an ancestor context
  /// </summary>
  bool IsInherited(const std::string& key);
  
  /// <summary>
  /// Get a reference to where the config value is stored
  /// </summary>
  /// <remarks>
  /// This method will throw an exception if the specified name cannot be found as a configurable value
  /// in the application, or if the specified value type does not match the type expected by this field
  /// </remarks>
  std::shared_ptr<AutoConfigVarBase> Get(const std::string& key);

  /// <summary>
  /// Assigns the specified value to an AutoConfig with a given key. Creates one if it doesn't exist.
  /// </summary>
  /// <remarks>
  /// This method will throw an exception if the specified name cannot be found as a configurable value
  /// in the application, or if the specified value type does not match the type expected by this field
  /// </remarks>
  template<class T>
  void Set(const std::string& key, const T& value) {
    
    if (!s_registry.count(key))
      ThrowKeyNotFoundException(key);
    
    if (!s_registry.find(key)->second->verifyType(typeid(T)))
      ThrowTypeMismatchException(key, typeid(T));
    
    SetInternal(key, &value);
  }
  
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
  
  // Add a callback for when key is changed in this context
  void AddOnChanged(const std::string& key, std::function<void(const AutoConfigVarBase&)>&& fx);

  // Add a callback for when a key is set in this context.  Is immediately called on all
  // currently existing values in the order they were created.
  onAddSignal_t::registration_t* AddCallback(onAddSignal_t::function_t&& fx);

  // Returns a list of all keys which have been set from this context.
  const std::vector<std::string>& GetLocalKeys() const { return m_orderedKeys; }

private:
  // Set a value in this manager, call callbacks
  // Must hold m_lock when calling this
  void SetInternal(const std::string& key, const void* value);

  std::shared_ptr<AutoConfigVarBase> GetOrConstruct(const std::string& key, const void* value);

  onAddSignal_t m_onAddedSignal;

  friend class AutoConfigVarBase;
  void NotifyConfigAdded(const std::shared_ptr<AutoConfigVarBase>& cfg);
};
