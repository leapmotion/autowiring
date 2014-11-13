// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include <string>
#include <typeinfo>

/// <summary>
/// Utility base type for configuration members
/// </summary>
class AutoConfigBase
{
public:
  template<class TMemberName>
  struct ConfigTypeExtractor {};

  AutoConfigBase(const std::type_info& tiMemberName);

  // Name of the class enclosing this configuration field
  const std::string Class;

  // Name of the type proper
  const std::string Name;
};

template<class T, class TMemberName>
class AutoConfig:
  public AutoConfigBase
{
public:
  AutoConfig(void) :
    AutoConfigBase(typeid(ConfigTypeExtractor<TMemberName>))
  {}

  T value;
};