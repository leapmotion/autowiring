// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoConfig.h"
#include "AnySharedPointer.h"
#include "demangle.h"
#include <iostream>

using namespace autowiring;

AutoConfigManager::AutoConfigManager(void){}

AutoConfigManager::~AutoConfigManager(void){}

void AutoConfigManager::Set(const char* name, const char* value) {
  Set(name, std::string(value));
}

void AutoConfigManager::SetParsed(const char *name, const char *value) {
  
}
