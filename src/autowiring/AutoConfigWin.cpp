// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoConfig.h"
#include "SlotInformation.h"
#include "demangle.h"
#include "expect.h"
#include <sstream>

static std::string CurrentStumpName(void) {
  const auto* cs = SlotInformationStackLocation::CurrentStump();
  return cs ? autowiring::demangle(cs->ti) : std::string();
}

static std::string ExtractFieldName(const std::type_info& ti) {
  // Name will be of the form "struct AutoConfigBase::ConfigTypeExtractor<struct MYCLASSNAME>"
  std::stringstream ss(ti.name());

  std::string retVal;
  if(ss >> autowiring::expect("struct")
        >> autowiring::expect("AutoConfigBase::ConfigTypeExtractor<struct")
  )
    ss >> retVal;
  return retVal;
}

AutoConfigBase::AutoConfigBase(const std::type_info& tiMemberName):
  Class(CurrentStumpName()),
  Name(ExtractFieldName(tiMemberName))
{
}