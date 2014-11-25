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
public:
  // Template paramaters reversed so optional namespace is last
  template<class... TField>
  struct ConfigTypeExtractor {};

  AutoConfigBase(const std::type_info& tiName);

  // Key used to identify this config value
  const std::string m_key;
};

template<class T, class... TField>
class AutoConfig:
  public AutoConfigBase
{
public:
  static_assert(!is_any<std::is_same<void, TField>...>::value, "void can't be used as a config name");
  static_assert(sizeof...(TField)==1 || sizeof...(TField)==2, "must provide a field and optional namespace");
  
  typedef ConfigTypeExtractor<TField...> t_field;
  
  AutoConfig(void) :
    AutoConfigBase(typeid(t_field)),
    m_value(m_manager->Get(m_key))
  {
    // Register with config registry
    (void)RegConfig<T, t_field>::r;
  }

private:
  AutoRequired<AutoConfigManager> m_manager;
  AnySharedPointer& m_value;

public:
  operator T(void){
    return m_value;
  }
  
  operator const T&(void){
    return m_value;
  }
  
  const T& operator*(void) const {
    return *m_value.as<T>();
  }

  /// <returns>
  /// True if this configurable field has been satisfied with a value
  /// </returns>
  bool IsConfigured(void) const {
    return !m_value->empty();
  }
};
