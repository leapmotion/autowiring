#include "stdafx.h"
#include "EnclosedContextTestBase.h"

EnclosedContextTestBase::EnclosedContextTestBase(void):
  m_pshr(m_create)
{
}

EnclosedContextTestBase::~EnclosedContextTestBase(void) {
  m_create->SignalShutdown();
  m_create->Wait();
}