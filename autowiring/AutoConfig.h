// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "Autowired.h"
#include "AutoConfigManager.h"
#include "ConfigRegistry.h"

#include <string>
#include TYPE_INDEX_HEADER

struct AnySharedPointer;

/// \internal
/// <summary>
/// Utility base type for configuration members
/// </summary>
class AutoConfigBase
{
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
public:
  static_assert(sizeof...(TKey) >= 1, "Must provide a key and optionally at least one namespace");
  
  template<typename t_Arg, typename ...t_Args>
  AutoConfig(t_Arg &&arg, t_Args&&... args) :
    AutoConfigBase(typeid(ConfigTypeExtractor<TKey...>))
  {
    // Register with config registry
    (void)RegConfig<T, TKey...>::r;

    if (!IsConfigured()){
      m_manager->Set(m_key, T(std::forward<t_Arg>(arg), std::forward<t_Args>(args)...));
    }
  }

  AutoConfig() :
    AutoConfigBase(typeid(ConfigTypeExtractor<TKey...>))
  {
    // Register with config registry
    (void)RegConfig<T, TKey...>::r;
  }

protected:
  AutoRequired<AutoConfigManager> m_manager;

public:
  const T& operator*() const {
    return *m_manager->Get(m_key).template as<T>().get();
  }

  const T* operator->(void) const {
    return m_manager->Get(m_key)->template as<T>().get();
  }

  void operator=(const T& newValue) {
    return m_manager->Set(m_key, newValue);
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
