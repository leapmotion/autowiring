// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoConfig.h"
#include "AnySharedPointer.h"
#include "demangle.h"
#include <iostream>
#include <vector>
#include <sstream>

using namespace autowiring;

static std::unordered_map<std::string, const ConfigRegistryEntry*> FillRegistry(void) {
  std::unordered_map<std::string, const ConfigRegistryEntry*> registry;
  
  for (auto config = g_pFirstConfigEntry; config; config = config->pFlink) {
    registry[config->m_key] = config;
  }
  
  return registry;
}

AutoConfigManager::AutoConfigManager(void):
  m_registry(FillRegistry())
{
  // Copy parents config settings
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

bool AutoConfigManager::IsConfigured(const std::string& key) {
  std::lock_guard<std::mutex> lk(m_lock);
  return !!m_values.count(key);
}

AnySharedPointer& AutoConfigManager::Get(const std::string& key) {
  std::lock_guard<std::mutex> lk(m_lock);
  
  // Check this first
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
  std::lock_guard<std::mutex> lk(m_lock);
  
  // Key not found
  if (!m_registry.count(key)) {
    return false;
  }
  
  SetInternal(key, m_registry.at(key)->parse(value));
  return true;
}

void AutoConfigManager::AddCallback(const std::string& key, t_callback&& fx) {
  std::lock_guard<std::mutex> lk(m_lock);
  m_callbacks[key].push_back(fx);
}

void AutoConfigManager::AddValidator(const std::string& key, t_validator&& validator) {
  std::lock_guard<std::mutex> lk(m_lock);
  if(m_values.count(key) && !validator(m_values[key])) {
    std::stringstream ss;
    ss << "Current value for key '" << key << "' is invalid";
    throw autowiring_error(ss.str());
  } else {
    m_validators[key].push_back(validator);
  }
}

void AutoConfigManager::SetRecursive(const std::string& key, AnySharedPointer value) {
  // Actually set the value in this manager
  SetInternal(key, value);
  
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
      std::lock_guard<std::mutex>(mgmt->m_lock);
    
      // Check if value set from this context
      // If so, stop recursing down this branch, continue to sibling
      if (mgmt->m_setHere.count(key)){
        ctxt.NextSibling();
        continue;
      }
      
      //Actaully set the value
      mgmt->SetInternal(key, value);
    
      // Continue to next context
      ++ctxt;
    }
  }
 
  // Mark that value was succefully set from this manager
  m_setHere.insert(key);
}

void AutoConfigManager::SetInternal(const std::string& key, const AnySharedPointer& value) {
  // Call all validators for this key
  for (auto const& fx : m_validators[key]) {
    if (!fx(value)){
      std::stringstream ss;
      ss << "Attempted to set key '" << key << "'which didin't pass validator";
      throw autowiring_error(ss.str());
    }
  }
  
  m_values[key] = value;
  
  // Call callbacks for this key
  for (const auto& cb : m_callbacks[key]) {
    cb(value);
  }
}
