// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoConfig.h"
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
static std::unordered_map<std::string, std::vector<AutoConfigManager::t_validator>> FillValidators(void) {
  std::unordered_map<std::string, std::vector<AutoConfigManager::t_validator>> validator;
  
  for (auto config = g_pFirstConfigEntry; config; config = config->pFlink) {
    if (config->m_has_validator) {
      validator[config->m_key].push_back(config->validator());
    }
  }
  
  return validator;
}

const std::unordered_map<std::string, const ConfigRegistryEntry*> AutoConfigManager::s_registry = FillRegistry();
const std::unordered_map<std::string, std::vector<AutoConfigManager::t_validator>> AutoConfigManager::s_validators = FillValidators();

AutoConfigManager::AutoConfigManager(void){
  // Copy parent's config settings
  auto parent = GetContext()->GetParentContext();
  if (parent) {
    AutowiredFast<AutoConfigManager> mgmt(parent);
    
    // Is there AutoConfigManager in an ancestor?
    if (mgmt) {
      std::lock_guard<std::mutex> lk(mgmt->m_lock);
      m_values = mgmt->m_values;
    }
  }
}

AutoConfigManager::~AutoConfigManager(void){}

void AutoConfigManager::ThrowKeyNotFoundException(const std::string& key) const {
  std::stringstream ss;
  ss << "No configuration found for key '" << key << "'";
  throw autowiring_error(ss.str());
}

void AutoConfigManager::ThrowTypeMismatchException(const std::string& key, const std::type_info& ti) const {
  std::stringstream ss;
  ss << "Attempting to set config '" << key << "' with incorrect type '"
    << autowiring::demangle(ti) << "'";
  throw autowiring_error(ss.str());

}

bool AutoConfigManager::IsConfigured(const std::string& key) {
  std::lock_guard<std::mutex> lk(m_lock);
  return !!m_values.count(key);
}

bool AutoConfigManager::IsInherited(const std::string& key) {
  std::lock_guard<std::mutex> lk(m_lock);
  return m_values.count(key) && !m_setHere.count(key);
}

AnySharedPointer& AutoConfigManager::Get(const std::string& key) {
  std::lock_guard<std::mutex> lk(m_lock);
  
  if (m_values.count(key)) {
    return m_values[key];
  }
  
  // Key not found, throw exception
  std::stringstream ss;
  ss << "Attepted to get key '" << key << "' which hasn't been set";
  throw autowiring_error(ss.str());
}

void AutoConfigManager::Set(const std::string& key, const char* value) {
  Set(key, std::string(value));
}

bool AutoConfigManager::SetParsed(const std::string& key, const std::string& value) {
  // Key not found
  if (!s_registry.count(key)) {
    return false;
  }
  
  SetRecursive(key, s_registry.find(key)->second->parse(value));
  return true;
}

void AutoConfigManager::AddCallback(const std::string& key, t_callback&& fx) {
  std::lock_guard<std::mutex> lk(m_lock);
  m_callbacks[key].push_back(fx);
}

void AutoConfigManager::SetRecursive(const std::string& key, AnySharedPointer value) {
  // Call all validators for this key
  if (s_validators.count(key)) {
    for (auto const& fx : s_validators.find(key)->second) {
      if (!fx(value)){
        std::stringstream ss;
        ss << "Attempted to set key '" << key << "'which didin't pass validator";
        throw autowiring_error(ss.str());
      }
    }
  }
  
  // Grab lock until done setting value
  std::lock_guard<std::mutex> lk(m_lock);
  
  // Actually set the value in this manager
  SetInternal(key, value);
  
  // Mark key set from this manager
  m_setHere.insert(key);
  
  // Enumerate descendant contexts
  auto enumerator = ContextEnumerator(GetContext());
  auto ctxt = enumerator.begin();
  
  // Skip current context
  ++ctxt;
  
  // Recursivly set values in desendent contexts
  while (ctxt != enumerator.end()) {
    
    // Make sure we get the AutoConfigManager from "ctxt"
    std::shared_ptr<AutoConfigManager> mgmt;
    (*ctxt)->FindByType(mgmt);
    if (mgmt) {
      std::lock_guard<std::mutex> descendant_lk(mgmt->m_lock);
    
      // Check if value was set from this context
      // If so, stop recursing down this branch, continue to next sibling
      if (mgmt->m_setHere.count(key)){
        ctxt.NextSibling();
        continue;
      }
      
      //Actaully set the value
      mgmt->SetInternal(key, value);
    }
    // Continue to next context
    ++ctxt;
  }
}

void AutoConfigManager::SetInternal(const std::string& key, const AnySharedPointer& value) {
  m_values[key] = value;
  
  // Call callbacks for this key
  for (const auto& cb : m_callbacks[key]) {
    cb(value);
  }
}
