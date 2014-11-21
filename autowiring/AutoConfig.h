// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "Autowired.h"
#include "AutoConfigManager.h"

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
    value(m_manager->m_attributes[Field])
  {}

private:
  AutoRequired<AutoConfigManager> m_manager;
  AnySharedPointer& value;

public:
  operator T(void){
    return value;
  }
  
  operator const T&(void){
    return value;
  }
  
  const T& operator*(void) const {
    return *value.as<T>();
  }

  /// <returns>
  /// True if this configurable field has been satisfied with a value
  /// </returns>
  bool IsConfigured(void) const {
    return false;
  }
};

