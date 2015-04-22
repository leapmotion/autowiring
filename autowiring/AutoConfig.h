// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoConfigBase.h"
#include "Autowired.h"
#include "ConfigRegistry.h"

/// <summary>
/// The type underlying the AutoConfig System.
/// Represents a unique type created by the combination of the type and a set of sigils.
/// Responsible for tracking changes to the underlying value and triggering signals,
/// making sure values are inherited correctly from enclosing contexts, and providing
/// a primitive polymorphic get/set interface (void*)
/// <summary>

template<class T, class... TKey>
class AutoConfigVar:
  public AutoConfigVarBase
{
public:
  static_assert(sizeof...(TKey) >= 1, "Must provide a key and optionally a set of namespaces");
  
  template<typename t_Arg, typename ...t_Args>
  AutoConfigVar(t_Arg &&arg, t_Args&&... args) :
    AutoConfigVarBase(typeid(ConfigTypeExtractor<TKey...>), true),
    m_value(std::forward<t_Arg>(arg), std::forward<t_Args>(args)...)
  {
    // Register with config registry
    (void)AutoConfigVar<T, TKey...>::RegistryEntry;

    if (RegistryEntry.m_hasValidator) {
      if (!RegistryEntry.validatorInternal()(m_value)) {
        throw autowiring_error("Cannot construct AutoConfigVar with a value that fails validation");
      }
    }

    onChangedSignal(*this);
  }

  AutoConfigVar() :
    AutoConfigVarBase(typeid(ConfigTypeExtractor<TKey...>)),
    m_value()
  {
    // Register with config registry
    (void)AutoConfigVar<T, TKey...>::RegistryEntry;
    
    const auto ctxt = m_context.lock();
    if (!ctxt)
      return;

    //This will wind up being recursive
    auto parent = ctxt->GetParentContext();
    if (parent != nullptr) {
      auto parentVar = parent->template Inject<AutoConfigVar<T, TKey...>>();

      //Only copy the value if it's initalized. Base::AutoInit will take care of
      //the various listing notifications.
      if (parentVar->IsConfigured()) {
        m_isConfigured = true;
        m_value = parentVar->m_value;
      }

      m_parentRegistration = *parentVar += [this](const T& val){
        RunValidation(val);
        SetInternal(val);
      };
    }
  }

public:
  
  operator const T&() const { return m_value; }
  const T* operator->() const { return &m_value; }

  void operator=(const T& newValue) {
    RunValidation(newValue);

    if (m_parentRegistration) {
      auto parent_ctxt = m_context.lock()->GetParentContext();
      AutowiredFast<AutoConfigVar<T, TKey...>> parentVar(parent_ctxt);
      *parentVar -= m_parentRegistration;
      m_parentRegistration = nullptr;
      OnSetLocally();
    }

    SetInternal(newValue);
  }

  void Get(void* pValue) const override { *reinterpret_cast<T*>(pValue) = m_value; }
  void Set(const void* pValue) override { *this = *reinterpret_cast<const T*>(pValue); }

  void SetParsed(const std::string& value) override { 
    *this = RegistryEntry.template parseInternal<T>(value);
  }

  // Add a callback for when this config value changes
  t_OnChangedSignal::registration_t* operator+=(std::function<void(const T&)>&& fx) {
    return onChangedSignal += [fx](const AutoConfigVarBase& var){
      fx(reinterpret_cast<const AutoConfigVar<T,TKey...>*>(&var)->m_value);
    };
  }

  void operator-=(t_OnChangedSignal::registration_t* node) { onChangedSignal -= node; }

private:
  T m_value;

  void RunValidation(const T& val) {
    if (RegistryEntry.m_hasValidator) {
      if (!RegistryEntry.validatorInternal()(val)) {
        throw autowiring_error("Validator rejected set for config value");
      }
    }
  }

  void SetInternal(const T& val) {
    m_isConfigured = true;
    m_value = val;
    onChangedSignal(*this);
  }

public:
  static std::shared_ptr<AutoConfigVarBase> Inject(const std::shared_ptr<CoreContext>& ctxt, const void* value) {
    if (!value)
      return ctxt->Inject<AutoConfigVar<T, TKey...>>();
    else
      return ctxt->Inject<AutoConfigVar<T, TKey...>>(*reinterpret_cast<const T*>(value));
  }

  static const ConfigRegistryEntryT<T, TKey...> RegistryEntry;
};

template<class T, class... TKey>
const ConfigRegistryEntryT<T, TKey...> AutoConfigVar<T, TKey...>::RegistryEntry(&AutoConfigVar<T, TKey...>::Inject);


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
/// AutoConfig values can also be set from the AutoConfigListing in the same context. The string key
/// is used as the identifier for the value.
/// </summary>
template<class T, class... TKeys>
class AutoConfig : public AutoRequired<AutoConfigVar<T, TKeys...>> {
public:
  typedef AutoConfigVar<T, TKeys...> t_Var;

  using AutoRequired<t_Var>::operator*;

  AutoConfig(const std::shared_ptr<CoreContext>& ctxt = CoreContext::CurrentContext()) :
    AutoRequired<t_Var>(ctxt)
  {
  }

  AutoConfig(T&& initialValue, const std::shared_ptr<CoreContext>& ctxt = CoreContext::CurrentContext()) :
    AutoRequired<t_Var>(ctxt, std::move(initialValue))
  {
    if (!(*this)->IsLocal()) {
      **this = std::move(initialValue);
    }
  }

  template<typename t_Arg, typename ...t_Args>
  explicit AutoConfig(t_Arg&& arg, t_Args&&... args) :
    AutoRequired<t_Var>(CoreContext::CurrentContext(), std::forward<t_Arg>(arg), std::forward<t_Args>(args)...)
  {
    //If we wind up being a reference to an existing value, we may still want to set it...
    if (!(*this)->IsLocal()) {
      **this = T(std::forward<t_Arg>(arg), std::forward<t_Args>(args)...);
    }
  }
  
};