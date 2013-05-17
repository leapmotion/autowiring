#include "stdafx.h"
#include "EnclosedContextTestBase.h"

void EnclosedContextTestBase::SetUp(void) {
  AutoCurrentContext current;
  m_create = current->Create();
  m_create->SetCurrent();
}

void EnclosedContextTestBase::TearDown(void) {
  // Cleanup time
  m_create->SignalShutdown();
  m_create->Wait();

  // Let the context tear down completely, now:
  m_create.reset();
}