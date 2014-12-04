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
  // Fill content from parent AutoConfigManagers
  for (auto ctxt = GetContext()->GetParentContext(); ctxt; ctxt = ctxt->GetParentContext()) {
    AutowiredFast<AutoConfigManager> mgmt;
    
    // Found closest AutoConfigManager, use its attributes and return
    if (mgmt) {
      std::lock_guard<std::mutex> lk(mgmt->m_lock);
      m_attributes = mgmt->m_attributes;
      return;
    }
  }
}

AutoConfigManager::~AutoConfigManager(void){}

bool AutoConfigManager::IsConfigured(const std::string& key) {
  std::lock_guard<std::mutex> lk(m_lock);
  return m_attributes.count(key);
}

AnySharedPointer& AutoConfigManager::Get(const std::string& key) {
  std::lock_guard<std::mutex> lk(m_lock);
  
  // Check this first
  if (m_attributes.count(key)) {
    return m_attributes[key];
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

void AutoConfigManager::AddCallback(const std::string& key, std::function<void(const AnySharedPointer&)>&& fx) {
  std::lock_guard<std::mutex> lk(m_lock);
  m_callbacks[key].push_back(fx);
}

void AutoConfigManager::SetInternal(const std::string& key, AnySharedPointer value) {
  // Set value and mark that value was set from here
  m_attributes[key] = value;
  m_setHere.insert(key);
  
  // Call callbacks for this key
  for (const auto& cb : m_callbacks[key]) {
    cb(value);
  }
  
  // Recursivly set values in desendent contexts
  for (const auto& ctxt : ContextEnumerator(GetContext())) {
    // We already set this context's value
    if (ctxt == GetContext())
      continue;
    
    AutowiredFast<AutoConfigManager> mgmt;
    if (mgmt) {
      std::lock_guard<std::mutex>(mgmt->m_lock);
      // Don't set value if 'mgmt' has set the value itself
      if (!mgmt->m_setHere.count(key)) {
        mgmt->m_attributes[key] = value;
        
        // Call callbacks for this key
        for (const auto& cb : m_callbacks[key]) {
          cb(value);
        }
      }
    }
  }
}
