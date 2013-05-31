// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "CoreThreadTest.h"
#include "Autowired.h"

class SpamguardTest:
  public CoreThread
{
public:
  SpamguardTest(void):
    m_hit(false),
    m_multiHit(false)
  {
    Ready();
  }

  bool m_hit;
  bool m_multiHit;

  void Run(void) override {
    if(m_hit) {
      m_multiHit = true;
      return;
    }

    m_hit = false;

    boost::unique_lock<boost::mutex> lk(m_lock);
    m_stateCondition.wait(lk, [this] () {return ShouldStop();});
  }
};

TEST_F(CoreThreadTest, VerifyStartSpam) {
  // Create our thread class:
  AutoRequired<SpamguardTest> instance;

  m_create->InitiateCoreThreads();

  // This shouldn't cause another thread to be created:
  instance->Start();

  EXPECT_FALSE(instance->m_multiHit) << "Thread was run more than once unexpectedly";
}
