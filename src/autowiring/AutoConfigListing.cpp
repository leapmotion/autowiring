// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoConfigBase.h"
#include "AutoConfigListing.h"
#include "AutoConfigParser.hpp"
#include "demangle.h"
#include <sstream>

using namespace autowiring;

// Create map of all config values in the registry
static std::unordered_map<std::string, const ConfigRegistryEntry*> FillRegistry(void) {
  std::unordered_map<std::string, const ConfigRegistryEntry*> registry;
  
  for (auto config = g_pFirstConfigEntry; config; config = config->pFlink) {
    registry[config->m_key] = config;
  }
  
  return registry;
}

// Create map of all validators specified
static std::unordered_map<std::string, AutoConfigListing::t_validator> FillValidators(void) {
  std::unordered_map<std::string, AutoConfigListing::t_validator> validator;
  
  for (auto config = g_pFirstConfigEntry; config; config = config->pFlink) {
    if (config->m_hasValidator) {
      validator[config->m_key] = config->validator();
    }
  }
  
  return validator;
}

const std::unordered_map<std::string, const ConfigRegistryEntry*> AutoConfigListing::s_registry = FillRegistry();
const std::unordered_map<std::string, AutoConfigListing::t_validator> AutoConfigListing::s_validators = FillValidators();

AutoConfigListing::AutoConfigListing(void){
  auto ctxt = m_context.lock();
}

AutoConfigListing::~AutoConfigListing(void){}

void AutoConfigListing::ThrowKeyNotFoundException(const std::string& key) const {
  std::stringstream ss;
  ss << "No configuration found for key '" << key << "'";
  throw autowiring_error(ss.str());
}

void AutoConfigListing::ThrowTypeMismatchException(const std::string& key, const std::type_info& ti) const {
  std::stringstream ss;
  ss << "Attempting to set config '" << key << "' with incorrect type '"
    << autowiring::demangle(ti) << "'";
  throw autowiring_error(ss.str());

}

bool AutoConfigListing::IsConfigured(const std::string& key) {
  std::lock_guard<std::mutex> lk(m_lock);
  auto found = m_values.find(key);

  return found != m_values.end() && found->second.lock()->IsConfigured();
}

bool AutoConfigListing::IsInherited(const std::string& key) {
  std::lock_guard<std::mutex> lk(m_lock);
  auto found = m_values.find(key);

  return found != m_values.end() && found->second.lock()->IsConfigured();
}

std::shared_ptr<AutoConfigVarBase> AutoConfigListing::Get(const std::string& key) {
  std::lock_guard<std::mutex> lk(m_lock);

  auto found = m_values.find(key);
  if (found != m_values.end())
    return found->second.lock();

  // Key not found, throw exception
  std::stringstream ss;
  ss << "Attepted to get key '" << key << "' which hasn't been set";
  throw autowiring_error(ss.str());
}

bool AutoConfigListing::SetParsed(const std::string& key, const std::string& value) {
  auto config = GetOrConstruct(key, nullptr);
  config->SetParsed(value);
  return true;
}

void AutoConfigListing::AddOnChanged(const std::string& key, std::function<void(const AutoConfigVarBase&)>&& fx) {
  auto config = GetOrConstruct(key, nullptr);
  (*config).onChangedSignal += std::move(fx);
}

AutoConfigListing::onAddSignal_t::registration_t* AutoConfigListing::AddCallback(onAddSignal_t::function_t&& fx) {
  std::lock_guard<std::mutex> lk(m_lock);

  for (auto& key : m_orderedKeys) {
    auto config = m_values[key].lock();
    if ( config )
      fx(*config);
  }
  
  return m_onAddedSignal += std::move(fx);
}

void AutoConfigListing::SetInternal(const std::string& key, const void* value) {
  auto config = GetOrConstruct(key, value);
  config->Set(value);
}

std::shared_ptr<AutoConfigVarBase> AutoConfigListing::GetOrConstruct(const std::string& key, const void* value) {
  std::unique_lock<std::mutex> lk(m_lock); 
  
  auto found = m_values.find(key);
  if (found != m_values.end())
    return found->second.lock();

  auto entry = s_registry.find(key);
  if (entry == s_registry.end()) {
    ThrowKeyNotFoundException(key);
    return nullptr;
  }

  auto ctxt = m_context.lock();
  lk.unlock();
  auto newValue = entry->second->m_injector(ctxt, value);
  lk.lock();
  
  m_values.emplace(key, newValue);
  return newValue;
}

void AutoConfigListing::NotifyConfigAdded(const std::shared_ptr<AutoConfigVarBase>& config){
  std::lock_guard<std::mutex> lk(m_lock);
  m_values.emplace(config->m_key, config);
}