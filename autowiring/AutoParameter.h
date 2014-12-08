// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoConfig.h"
#include "has_validate.h"

/// <summary>
/// Register an AutoParameter with "AutoParam" namespace in AutoConfigManager.
/// In addition to being the lookup string, the Key also implements:
///   - static constexpr T Default()
///   - (optional) static bool Validate(const T&)
///
/// AutoParameter uses AutoConfig under the hood and will use "AutoParam" as
/// its namespace
/// </summary>
template<class T, class TKey>
class AutoParameter:
  public AutoConfig<T, struct AutoParam, TKey>
{
public:
  AutoParameter() :
    AutoConfig<T, struct AutoParam, TKey>()
  {
    T defaultValue = TKey::Default();
    if (!CallValidate<T, TKey>(defaultValue, has_validate<TKey>())) {
      throw autowiring_error("invalid value for key: " + this->m_key);
    }
    Set(defaultValue);
  }
  
  bool Set(const T& value) {
    if (!CallValidate<T, TKey>(value, has_validate<TKey>())) {
      return false;
    }
    
    this->m_manager->Set(this->m_key, value);
    return true;
  }
};
