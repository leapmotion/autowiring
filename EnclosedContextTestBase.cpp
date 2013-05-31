// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "EnclosedContextTestBase.h"

EnclosedContextTestBase::EnclosedContextTestBase(void):
  m_pshr(m_create)
{
}

EnclosedContextTestBase::~EnclosedContextTestBase(void) {
  m_create->SignalShutdown();

  // Do not allow teardown to take more than a millisecond
  if(!m_create->Wait(static_cast<boost::chrono::duration<double, boost::milli> >(100.))) {
    ASSERT(false);
  }
}
