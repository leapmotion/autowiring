// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "Autowired.h"
#include "AutoConfigManager.h"
#include "ConfigRegistry.h"
#include "is_any.h"

#include <string>
#include <typeinfo>

struct AnySharedPointer;

/// <summary>
/// Utility base type for configuration members
/// </summary>
class AutoConfigBase
{
protected:
  // Template arguemnts TKey specify the key and optional namespace for a config attribute
  template<class... TKey>
  struct ConfigTypeExtractor {};
  
public:
  AutoConfigBase(const std::type_info& tiName);

  // Key used to identify this config value
  const std::string m_key;
};

/// <summary>
/// Register an attribute with the AutoConfig system. For example
///
/// AutoConfig<int, struct MyNamespace, struct MyKey> m_myVal;
/// defines the key "MyNamespace.MyKey"
///
/// The Namespace field is optional, so
/// AutoConfig<int, struct MyKey> m_myVal;
/// defines the key "MyKey"
///
/// AutoConfig values can be set from the AutoConfigManager. The string key
/// is used as the identifier for the value
/// </summary>
template<class T, class... TKey>
class AutoConfig:
  public AutoConfigBase
{
private:
  // Specifies the optional namespace and key for this config attribute
  typedef ConfigTypeExtractor<TKey...> t_field;
  
public:
  static_assert(sizeof...(TKey)==1 || sizeof...(TKey)==2, "Must provide a key and optional namespace");
  
  AutoConfig(void) :
    AutoConfigBase(typeid(t_field))
  {
    // Register with config registry
    (void)RegConfig<T, t_field>::r;
  }

private:
  AutoRequired<AutoConfigManager> m_manager;

public:
  const T& operator*() const {
    return *m_manager->Get(m_key).template as<T>().get();
  }

  const T* operator->(void) const {
    return m_manager->Get(m_key)->template as<T>().get();
  }

  /// <returns>
  /// True if this configurable field has been satisfied with a value
  /// </returns>
  bool IsConfigured(void) const {
    return m_manager->IsConfigured(m_key);
  }
  
  // Add a callback for when this config value changes
  void operator+=(std::function<void(const T&)>&& fx) {
    m_manager->AddCallback(m_key, [fx](const AnySharedPointer& val){
      fx(*val.template as<T>().get());
    });
  }
};
