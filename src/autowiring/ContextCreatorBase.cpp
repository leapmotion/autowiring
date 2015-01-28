// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "ContextCreatorBase.h"
#include "CoreContext.h"

ContextCreatorBase::ContextCreatorBase(void):
  m_context(CoreContext::CurrentContext())
{}

ContextCreatorBase::~ContextCreatorBase(void) {}
