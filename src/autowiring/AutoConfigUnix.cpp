// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoConfig.h"
#include "SlotInformation.h"

AutoConfigBase::AutoConfigBase(const std::type_info& tiMemberName):
  Class(nullptr),
  Name(nullptr)
{
  const char* name = tiMemberName.name();
}