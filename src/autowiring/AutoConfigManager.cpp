// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoConfig.h"
#include "AnySharedPointer.h"
#include "demangle.h"
#include <iostream>
#include <vector>
#include <sstream>

using namespace autowiring;

AutoConfigManager::AutoConfigManager(void){}
AutoConfigManager::~AutoConfigManager(void){}

bool AutoConfigManager::IsConfigured(const std::string& key) {
  // iterate ancestor contexts, filling any configs
  for (auto ctxt = GetContext(); ctxt; ctxt = ctxt->GetParentContext()) {
    AutowiredFast<AutoConfigManager> mgmt(ctxt);
    
    if(mgmt) {
      std::lock_guard<std::mutex> lk(mgmt->m_lock);
      if (mgmt->m_attributes.count(key)) {
        return true;
      }
    }
  }
  
  // Key not found
  return false;
}

AnySharedPointer& AutoConfigManager::Get(const std::string& key) {
  std::lock_guard<std::mutex> lk(m_lock);
  
  // Check this first
  if (m_attributes.count(key)) {
    return m_attributes[key];
  }
  
  // iterate ancestor contexts, filling any configs
  for (auto ctxt = GetContext()->GetParentContext(); ctxt; ctxt = ctxt->GetParentContext()) {
    AutowiredFast<AutoConfigManager> mgmt(ctxt);
    
    if(mgmt) {
      std::lock_guard<std::mutex> lk(mgmt->m_lock);
      if (mgmt->m_attributes.count(key)) {
        return mgmt->m_attributes[key];
      }
    }
  }
  
  // Key not found, throw exception
  std::stringstream ss;
  ss << "Attepted to get key '" << key << "' which hasn't been set";
  throw autowiring_error(ss.str());
}

void AutoConfigManager::Set(const std::string& key, const char* value) {
  Set(key, std::string(value));
}

void AutoConfigManager::SetParsed(const std::string& key, const std::string& value) {
  std::lock_guard<std::mutex> lk(m_lock);
  
  for (auto config = g_pFirstConfigEntry; config; config = config->pFlink) {
    if (config->is(key)){
      m_attributes[key] = config->parse(value);
      return;
    }
  }
  
  // Error if key wasn't found in registry
  std::stringstream ss;
  ss << "Cant parse '" << value <<"' for key '" << key << "'";
  throw autowiring_error(ss.str());
}
