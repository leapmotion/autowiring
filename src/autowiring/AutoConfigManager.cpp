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
      m_attributes = mgmt->m_attributes;
    }
  }
}

AutoConfigManager::~AutoConfigManager(void){}

bool AutoConfigManager::IsConfigured(const std::string& key) {
  std::lock_guard<std::mutex> lk(m_lock);
  return !!m_attributes.count(key);
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

void AutoConfigManager::AddCallback(const std::string& key, t_callback&& fx) {
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
    
      mgmt->m_attributes[key] = value;
    
      for (const auto& cb : mgmt->m_callbacks[key])
        cb(value);
    
      // Continue to next context
      ++ctxt;
    }
  }
}
