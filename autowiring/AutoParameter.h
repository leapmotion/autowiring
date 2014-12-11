// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoConfig.h"
#include "has_validate.h"
#include <type_traits>

struct AutoParam{};

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
  public AutoConfig<T, AutoParam, TKey>
{
public:
  static_assert(std::is_constructible<TKey>::value, "Cannot use the default keys provided. You must subclass and use your own");
  
  AutoParameter() :
    AutoConfig<T, AutoParam, TKey>(),
    m_default(TKey::Default())
  {
    if (!isValid(m_default)) {
      throw autowiring_error("invalid default value for key: " + this->m_key);
    }
  }
  
  const T& operator*() const {
    return
      this->IsConfigured() ?
      AutoConfig<T, AutoParam, TKey>::operator*() :
      m_default;
  }
  
  const T* operator->(void) const {
    return this->IsConfigured() ?
      this->template AutoConfig<T, AutoParam, TKey>::operator->() :
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
    return CallValidate<T, TKey>::Call(value);
  }
};

/// <summary>
/// Base class for providing an easy way to specify just a default value
/// </summary>
/// <remarks>
/// Because the class names are used as keys to be stored in the AutoConfigManager, these classes cannot be used
/// directly and must be subclassed:
///
///   struct MyDefaultKey : DefaultKey<int, 15> {};
///   AutoParameter<int, MyDefaultKey> param;
///
/// </remarks>
template<typename T, T DEFAULT>
struct DefaultKey
{
public:
  static T Default() { return DEFAULT; }
  
protected:
  virtual ~DefaultKey() = 0;
};

/// <summary>
/// Base class for providing an easy way to default, min and max values
/// </summary>
/// <remarks>
/// Because the class names are used as keys to be stored in the AutoConfigManager, these classes cannot be used
/// directly and must be subclassed:
///
///   struct MyDefaultMinMaxKey : DefaultMinMaxKey<int, 15, 10, 20> {};
///   AutoParameter<int, MyDefaultMinMaxKey> param;
///
/// </remarks>
template<typename T, T DEFAULT, T MIN, T MAX>
struct DefaultMinMaxKey : public DefaultKey<T, DEFAULT>
{
public:
  static_assert(MIN <= DEFAULT && DEFAULT <= MAX, "Default value must be within range");
  
  static bool Validate(const T& value) {
    return MIN <= value && value <= MAX;
  }
  
protected:
  virtual ~DefaultMinMaxKey() = 0;
};
