// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoConfig.h"
#include "AutoConfigListing.h"
#include "AnySharedPointer.h"
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
  auto config = m_values[key].lock();
  return config && config->IsConfigured();
}

bool AutoConfigListing::IsInherited(const std::string& key) {
  std::lock_guard<std::mutex> lk(m_lock);
  auto config = m_values[key].lock();
  return config && config->IsInherited();
}

std::weak_ptr<AutoConfigVarBase> AutoConfigListing::Get(const std::string& key) {
  std::lock_guard<std::mutex> lk(m_lock);
  return m_values[key];
  
  // Key not found, throw exception
  std::stringstream ss;
  ss << "Attepted to get key '" << key << "' which hasn't been set";
  throw autowiring_error(ss.str());
}

bool AutoConfigListing::SetParsed(const std::string& key, const std::string& value) {
  // Key not found
  if (!s_registry.count(key)) {
    return false;
  }

  auto parsedValue = s_registry.find(key)->second->parse(value);
  
  SetInternal(key, &(*parsedValue));
  return true;
}

void AutoConfigListing::AddOnChanged(const std::string& key, std::function<void(const AutoConfigVarBase&)>&& fx) {
  std::lock_guard<std::mutex> lk(m_lock);
  auto config = m_values[key].lock();
  if (config)
    (*config).onChangedSignal += std::move(fx);
}

void AutoConfigListing::AddCallback(t_add_callback&& fx) {
  // Grab lock until done setting value
  std::lock_guard<std::mutex> lk(m_lock);

  for (auto& key : m_orderedKeys) {
    auto config = m_values[key].lock();
    if ( config )
      fx(*config);
  }
  
  m_addCallbacks.emplace_back(std::move(fx));
}

void AutoConfigListing::SetInternal(const std::string& key, const void* value) {
  std::unique_lock<std::mutex> lk(m_lock);
  auto config = m_values[key].lock();

  if (!config) {
    auto entry = s_registry.find(key);
    if (entry == s_registry.end()) {
      ThrowKeyNotFoundException(key);
      return;
    }

    auto ctxt = m_context.lock();
    lk.unlock();
    config = entry->second->m_injector(ctxt, value);
    lk.lock();
    m_values[key] = config;
  }

  lk.unlock();
  config->Set(value);
}
