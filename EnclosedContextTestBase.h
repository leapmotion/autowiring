// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _ENCLOSED_CONTEXT_TEST_BASE_H
#define _ENCLOSED_CONTEXT_TEST_BASE_H
#include "Autowired.h"
#include <gtest/gtest.h>
#include <memory>

class EnclosedContextTestBase:
  public testing::Test
{
public:
  EnclosedContextTestBase(void) :
    m_pshr(m_create),
    m_createWeak(m_create)
  {}

  ~EnclosedContextTestBase(void) {
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


protected:
  // The context proper.  This is automatically assigned as the current
  // context when SetUp is invoked.
  AutoCreateContext m_create;

  // Currency pusher:
  CurrentContextPusher m_pshr;
 
private:
  std::weak_ptr<CoreContext> m_createWeak;
};

#endif
