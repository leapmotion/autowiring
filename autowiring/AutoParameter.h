// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoConfig.h"

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
    Set(TKey::Default());
    // TODO:
    //    REQUIRED: default value.
    //    OPTIONAL: validator.
    //    Make sure
  }
  
  bool Set(const T& value) {
    // TODO: validate, set, etc
    
    this->m_manager->Set(this->m_key, value);
    return true;
  }
};
