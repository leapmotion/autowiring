// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "Autowired.h"
#include "auto_signal.h"
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
  
  typedef autowiring::signal<void(void)> t_OnChangedSignal;
  t_OnChangedSignal onChangedSignal;
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
  }

  AutoConfig() :
    AutoConfigBase(typeid(ConfigTypeExtractor<TKey...>))
  {
    // Register with config registry
    (void)RegConfig<T, TKey...>::r;
  }

public:
  
  operator const T&() const { return m_value; }

  void operator=(const T& newValue) {
    m_value = newValue;
  }

  // Add a callback for when this config value changes
  t_OnChangedSignal::t_registration* operator+=(std::function<void(const T&)>&& fx) {
    return onChangedSignal += [this,fx](){
      fx(m_value);
    };
  }

private:
  T m_value;
};
