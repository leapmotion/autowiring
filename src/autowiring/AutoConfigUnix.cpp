// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoConfig.h"
#include "SlotInformation.h"
#include "demangle.h"
#include <regex>
#include <iostream>

using namespace autowiring;

static const std::regex namePattern(".*ConfigTypeExtractor<(?:class |struct )?(\\w*)>");
static const std::regex classPattern(".*TypeUnifierComplex<(?:class |struct )?(\\w*)>");

static std::string ExtractFieldName(const std::type_info& ti) {
  std::smatch sm;
  std::regex_match(demangle(ti), sm, namePattern);
  assert(sm.size() == 2 && "Regex couldn't find type name");
  return sm.str(1);
}

static std::string CurrentStumpName(void) {
  const SlotInformationStumpBase* cs = SlotInformationStackLocation::CurrentStump();
  if (!cs)
    return std::string();

  std::smatch sm;
  std::regex_match(demangle(cs->ti), sm, classPattern);
  assert(sm.size() == 2 && "Regex couldn't find class name");
  return sm.str(1);
}

AutoConfigBase::AutoConfigBase(const std::type_info& tiMemberName):
  Class(CurrentStumpName()),
  Name(ExtractFieldName(tiMemberName))
{
  
}
