// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "Autowired.h"
#include "AutoConfigManager.h"
#include "ConfigRegistry.h"

#include <string>
#include <typeinfo>

struct AnySharedPointer;

/// <summary>
/// Utility base type for configuration members
/// </summary>
class AutoConfigBase
{
public:
  template<class TMemberName>
  struct ConfigTypeExtractor {};

  AutoConfigBase(const std::type_info& tiName);

  // Name of the class enclosing this configuration field
  const std::string Class;

  // Name of the type proper
  const std::string Name;
  
  // Concatinated field name
  const std::string Field;
};

template<class T, class TMemberName>
class AutoConfig:
  public AutoConfigBase
{
public:
  AutoConfig(void) :
    AutoConfigBase(typeid(ConfigTypeExtractor<TMemberName>)),
    m_value(m_manager->Get(Field))
  {
    // Register with config registry
    (void)RegConfig<ConfigTypeExtractor<TMemberName>>::r;
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

