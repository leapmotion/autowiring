// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "EnclosedContextTestBase.h"

EnclosedContextTestBase::EnclosedContextTestBase(void):
  m_pshr(m_create)
{
  m_createWeak = m_create;
}

EnclosedContextTestBase::~EnclosedContextTestBase(void) {
  // Only attempt teardown if it hasn't already happened:
  auto ctxt = m_createWeak.lock();
  if(!ctxt)
    return;

  ctxt->SignalShutdown();

  // Do not allow teardown to take more than a millisecond
  if(!ctxt->Wait(static_cast<boost::chrono::duration<double, boost::milli> >(100.))) {
    ASSERT(false);
  }
}
