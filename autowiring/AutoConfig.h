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
class AutoConfigVarBase
{
public:
  AutoConfigVarBase(const std::type_info& tiName);

  // Key used to identify this config value
  const std::string m_key;

  // Generic interface functions for getting and setting
  virtual void Get(void* pValue) const = 0;
  virtual void Set(const void* pValue) = 0;
  
  typedef autowiring::signal<void(const AutoConfigVarBase& val)> t_OnChangedSignal;
  t_OnChangedSignal onChangedSignal;
};

template<class T, class... TKey>
class AutoConfigVar:
  public AutoConfigVarBase
{
public:
  static_assert(sizeof...(TKey) >= 1, "Must provide a key and optionally at least one namespace");
  
  template<typename t_Arg, typename ...t_Args>
  AutoConfigVar(t_Arg &&arg, t_Args&&... args) :
    AutoConfigVarBase(typeid(ConfigTypeExtractor<TKey...>)),
    m_value(std::forward<t_Arg>(arg), std::forward<t_Args>(args)...)
  {
    // Register with config registry
    (void)RegConfig<T, TKey...>::r;
  }

  AutoConfigVar() :
    AutoConfigVarBase(typeid(ConfigTypeExtractor<TKey...>))
  {
    // Register with config registry
    (void)RegConfig<T, TKey...>::r;
  }

public:
  
  operator const T&() const { return m_value; }

  void operator=(const T& newValue) {
    m_value = newValue;
  }

  void Get(void* pValue) const override { *reinterpret_cast<T*>(pValue) = m_value; }
  void Set(const void* pValue) override { m_value = *reinterpret_cast<const T*>(pValue); }

  // Add a callback for when this config value changes
  t_OnChangedSignal::t_registration* operator+=(std::function<void(const T&)>&& fx) {
    return onChangedSignal += [this,fx](){
      fx(m_value);
    };
  }

private:
  T m_value;
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
template<class T, class... TKeys>
class AutoConfig : public AutoRequired<AutoConfigVar<T, TKeys...>> {
public:
  AutoConfig(const std::shared_ptr<CoreContext>& ctxt = CoreContext::CurrentContext()) :
    AutoRequired<AutoConfigVar<T, TKeys...>>(ctxt)
  {
  }

  template<typename ...t_Args>
  AutoConfig(const std::shared_ptr<CoreContext>& ctxt, t_Args&&... args) :
    AutoRequired<AutoConfigVar<T, TKeys...>>(ctxt, std::forward<t_Args>(args)...)
  {
  }
};