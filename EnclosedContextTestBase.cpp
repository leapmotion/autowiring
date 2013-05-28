#include "stdafx.h"
#include "EnclosedContextTestBase.h"

EnclosedContextTestBase::EnclosedContextTestBase(void):
  m_pshr(m_create)
{
}

EnclosedContextTestBase::~EnclosedContextTestBase(void) {
  m_create->SignalShutdown();

  // Do not allow teardown to take more than a millisecond
  if(!m_create->Wait(boost::chrono::milliseconds(100)))
    ASSERT(false);
}
