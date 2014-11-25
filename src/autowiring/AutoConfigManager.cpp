// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoConfig.h"
#include "AnySharedPointer.h"
#include "demangle.h"
#include <iostream>
#include <vector>
#include <sstream>

using namespace autowiring;

AutoConfigManager::AutoConfigManager(void){
  std::shared_ptr<CoreContext> ctxt = GetContext()->GetParentContext();
  
  // iterate ancestor contexts, filling any configs
  while (ctxt) {
    AutowiredFast<AutoConfigManager> mgmt(ctxt);
    
    if(mgmt) {
      std::lock_guard<std::mutex> lk(mgmt->m_lock);
      for (const auto& entry : mgmt->m_attributes)
        m_attributes.insert(entry);
    }

    ctxt = ctxt->GetParentContext();
  }
}

AutoConfigManager::~AutoConfigManager(void){}

AnySharedPointer& AutoConfigManager::Get(const std::string& key) {
  std::lock_guard<std::mutex> lk(m_lock);
  return m_attributes[key];
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
  ss << "Cant parse '" << value <<"' for key '" << key << "'.";
  throw autowiring_error(ss.str().c_str());
}
