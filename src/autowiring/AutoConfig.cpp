// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoConfig.h"
#include "AutoConfigParser.hpp"

AutoConfigVarBase::AutoConfigVarBase(const std::type_info& ti) :
  m_key(autowiring::ExtractKey(ti))
  onChangedSignal(),
{}
