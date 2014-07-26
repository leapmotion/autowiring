// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "NewAutoFilter.h"
#include "SlotInformation.h"
#include "autowiring_error.h"

NewAutoFilterBase::NewAutoFilterBase(const AutoFilterDescriptorStub& stub) :
  m_stub(stub)
{
  auto location = SlotInformationStackLocation::CurrentStackLocation();

  // Verify that there actually is a stack location
  if(!location)
    throw autowiring_error("Attempted to make use of an AutoFilter entry without creating a type");

  // Verify that our created slot is in the body of the enclosing class
  if(!location->Encloses(this))
    throw autowiring_error("AutoFilter entries may only be constructed precisely in the _body_ of a class, never on the stack");

  auto stump = location->GetStump();
  pFlink = stump->pFirstAutoFilter;
  stump->pFirstAutoFilter = this;
}
