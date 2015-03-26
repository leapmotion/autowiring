// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoConfigBase.h"
#include "AutoConfigParser.hpp"

AutoConfigVarBase::AutoConfigVarBase(const std::type_info& ti, bool configured) :
  ContextMember(),
  m_key(autowiring::ExtractKey(ti)),
  onChangedSignal(),
  m_isConfigured(configured),
  m_parentRegistration(nullptr)
{
  m_name = m_key.c_str();
}

