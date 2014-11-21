// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include <string>
#include STL_UNORDERED_MAP

struct AnySharedPointer;

class AutoConfigManager {
  
private:
  std::unordered_map<std::string, AnySharedPointer> m_attributes;
};