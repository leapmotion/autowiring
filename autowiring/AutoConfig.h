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
class AutoConfigVarBase : public ContextMember
{
public:
  AutoConfigVarBase(const std::type_info& tiName, bool configured = false);

  // Key used to identify this config value
  const std::string m_key;

  // Generic interface functions for getting and setting
  virtual void Get(void* pValue) const = 0;
  virtual void Set(const void* pValue) = 0;

  virtual void SetParsed(const std::string& value) = 0;

  typedef autowiring::signal<void(const AutoConfigVarBase& val)> t_OnChangedSignal;
  t_OnChangedSignal onChangedSignal;

  bool IsConfigured() const { return m_isConfigured; }
  bool IsInherited() const { return m_parentRegistration != nullptr; }

protected:
  bool m_isConfigured;
  t_OnChangedSignal::t_registration* m_parentRegistration;
};

template<class T, class... TKey>
class AutoConfigVar:
  public AutoConfigVarBase
{
public:
  static_assert(sizeof...(TKey) >= 1, "Must provide a key and optionally at least one namespace");
  
  template<typename t_Arg, typename ...t_Args>
  AutoConfigVar(t_Arg &&arg, t_Args&&... args) :
    AutoConfigVarBase(typeid(ConfigTypeExtractor<TKey...>), true),
    m_value(std::forward<t_Arg>(arg), std::forward<t_Args>(args)...)
  {
    // Register with config registry
    (void)RegConfig<T, TKey...>::r;

    onChangedSignal(*this);
  }

  AutoConfigVar() :
    AutoConfigVarBase(typeid(ConfigTypeExtractor<TKey...>)),
    m_value()
  {
    // Register with config registry
    (void)RegConfig<T, TKey...>::r;
    
    const auto ctxt = m_context.lock();
    if (!ctxt)
      return;

    //This will wind up being recursive
    auto parent = ctxt->GetParentContext();
    if (parent != nullptr) {
      auto parentVar = parent->Inject<AutoConfigVar<T, TKey...>>();

      //Only copy the value if it's initalized
      if (parentVar->IsConfigured()) {
        SetInternal(parentVar->m_value);
      }

      m_parentRegistration = *parentVar += [this](const T& val){
        SetInternal(val);
      };
    }
  }

public:
  
  operator const T&() const { return m_value; }
  const T* operator->() const { return &m_value; }

  void operator=(const T& newValue) {
    if (m_parentRegistration) {
      auto parent_ctxt = m_context.lock()->GetParentContext();
      AutowiredFast<AutoConfigVar<T, TKey...>> parentVar(parent_ctxt);
      *parentVar -= m_parentRegistration;
      m_parentRegistration = nullptr;
    }
    SetInternal(newValue);
  }

  void Get(void* pValue) const override { *reinterpret_cast<T*>(pValue) = m_value; }
  void Set(const void* pValue) override { *this = *reinterpret_cast<const T*>(pValue); }

  void SetParsed(const std::string& value) override { 
    auto entry = RegConfig<T, TKey...>::r;
    *this = entry.parseInternal<T>(value);
  }

  // Add a callback for when this config value changes
  t_OnChangedSignal::t_registration* operator+=(std::function<void(const T&)>&& fx) {
    return onChangedSignal += [fx](const AutoConfigVarBase& var){
      fx(reinterpret_cast<const AutoConfigVar<T,TKey...>*>(&var)->m_value);
    };
  }

  void operator-=(t_OnChangedSignal::t_registration* node) { onChangedSignal -= node; }

private:
  T m_value;

  void SetInternal(const T& val) {
    m_isConfigured = true;
    m_value = val;
    onChangedSignal(*this);
  }
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
  typedef AutoConfigVar<T, TKeys...> t_Var;

  AutoConfig(const std::shared_ptr<CoreContext>& ctxt = CoreContext::CurrentContext()) :
    AutoRequired<t_Var>(ctxt)
  {
  }

  template<typename ...t_Args>
  AutoConfig(const std::shared_ptr<CoreContext>& ctxt, t_Args&&... args) :
    AutoRequired<t_Var>(ctxt, std::forward<t_Args>(args)...)
  {
  }

  using AutoRequired<t_Var>::operator*;
};