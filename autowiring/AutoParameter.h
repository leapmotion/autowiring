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
struct AutoParam{};

template<class T, class TKey>
class AutoParameter:
  public AutoConfig<T, struct AutoParam, TKey>
{
public:
  AutoParameter() :
    AutoConfig<T, struct AutoParam, TKey>(),
    m_default(TKey::Default())
  {
    if (!isValid(m_default)) {
      throw autowiring_error("invalid default value for key: " + this->m_key);
    }

    if (this->IsConfigured() && isValid(this->template operator*())) {
      throw autowiring_error("currently configured value is invalid for key: " + this->m_key);
    }
  }
  
  const T& operator*() const {
    return this->IsConfigured() ?
      this->template AutoConfig<T, struct AutoParam, TKey>::operator*() :
      m_default;
  }
  
  const T* operator->(void) const {
    return this->IsConfigured() ?
      this->template AutoConfig<T, struct AutoParam, TKey>::operator->() :
      &m_default;
  }
  
  bool Set(const T& value) {
    if (!isValid(value)) {
      return false;
    }
    
    this->m_manager->Set(this->m_key, value);
    return true;
  }
  
protected:
  const T m_default;
  
  bool isValid(const T& value) const {
    return CallValidate<T, TKey>(value, has_validate<TKey>());
  }
};
