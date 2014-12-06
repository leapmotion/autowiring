// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoConfig.h"

/// <summary>
/// Register an AutoParameter with "AutoParam" namespace in AutoConfigManager.
/// In addition to being the lookup string, the Key also:
///   - Contains the default value
///   - Supply a validator function (optional)
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
    // TODO:
    //    REQUIRED: default value.
    //    OPTIONAL: validator.
    //    Make sure
  }
  
  void Reset() {
    // TODO: Reset to default
  }
  
  bool Set(const T& value) {
    // TODO: validate, set, etc
    
    this->m_manager->Set(this->m_key, value);
    return true;
  }
};
