// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoConfig.h"
#include "AnySharedPointer.h"
#include "demangle.h"
#include <iostream>

using namespace autowiring;

AutoConfigManager::AutoConfigManager(void){}

AutoConfigManager::~AutoConfigManager(void){}

AnySharedPointer& AutoConfigManager::Get(const std::string& name) {
  return m_attributes[name];
}

void AutoConfigManager::Set(const std::string& name, const char* value) {
  Set(name, std::string(value));
}

void AutoConfigManager::SetParsed(const std::string& name, const std::string& value) {
  
}
