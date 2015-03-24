// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoConfig.h"
#include "AutoConfigParser.hpp"

AutoConfigVarBase::AutoConfigVarBase(const std::type_info& ti, bool configured) :
  m_key(autowiring::ExtractKey(ti)),
  m_isConfigured(configured),
  onChangedSignal(),
  ContextMember(m_key.c_str())
{}

