// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ContextMember.h"
#include "CoreContext.h"

ContextMember::ContextMember(const char* name):
  m_context(CoreContext::CurrentContext()),
  m_name(name)
{}

ContextMember::~ContextMember() {}
