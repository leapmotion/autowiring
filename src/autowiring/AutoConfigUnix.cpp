// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoConfig.h"
#include "AutoConfigParser.hpp"

AutoConfigBase::AutoConfigBase(const std::type_info& ti):
  m_key(autowiring::ExtractKey(ti))
{}
