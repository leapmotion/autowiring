// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoConfig.h"
#include "AnySharedPointer.h"
#include "demangle.h"
#include <iostream>
#include <vector>

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

AnySharedPointer& AutoConfigManager::Get(const std::string& name) {
  std::lock_guard<std::mutex> lk(m_lock);
  return m_attributes[name];
}

void AutoConfigManager::Set(const std::string& name, const char* value) {
  Set(name, std::string(value));
}

void AutoConfigManager::SetParsed(const std::string& name, const std::string& value) {
  
}
