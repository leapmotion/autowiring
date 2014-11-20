// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoConfig.h"
#include "SlotInformation.h"
#include "demangle.h"
#include <regex>
#include <iostream>

using namespace autowiring;

static const std::regex namePattern("AutoConfigBase::ConfigTypeExtractor<(\\w*)>");

static std::string getName(const std::type_info& ti) {
  std::smatch sm;
  std::regex_match(demangle(ti), sm, namePattern);
  assert(sm.size() == 2 && "Regex couldn't find type name");
  return sm.str(1);
}

AutoConfigBase::AutoConfigBase(const std::type_info& tiMemberName):
  Name(getName(tiMemberName))
{}