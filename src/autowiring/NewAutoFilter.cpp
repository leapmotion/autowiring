// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "NewAutoFilter.h"
#include "SlotInformation.h"
#include "autowiring_error.h"

NewAutoFilterBase::NewAutoFilterBase(const AutoFilterDescriptorStub& stub)
{
  auto location = SlotInformationStackLocation::CurrentStackLocation();

  // Verify that there actually is a stack location
  if(!location)
    throw autowiring_error("Attempted to make use of an AutoFilter entry without creating a type");

  location->RegisterSlot(stub);
}
