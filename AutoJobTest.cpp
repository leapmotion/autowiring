#include "stdafx.h"
#include "AutoJobTest.h"
#include "AutoJob.h"

TEST_F(AutoJobTest, DISABLED_VerifySimpleProperties) {
  // We expect to block forever here, this is OK:
  ASSERT_FALSE(m_create->Wait(boost::chrono::milliseconds(1))) << "Initial context wait check returned prematurely";

  AutoRequired<AutoJob> jb;

  //ASSERT_FALSE(m_create->IsInitiated()) << "AutoJob reported it could receive events before its enclosing context was created";

  // Create a thread which will delay for acceptance, and then quit:
  boost::thread t([&jb] {
    //jb->DelayUntilInitiated();
  });

  // Verify that this thread doesn't back out right away:
  ASSERT_FALSE(t.try_join_for(boost::chrono::milliseconds(10))) << "AutoJob did not block a client who was waiting for its readiness to accept dispatchers";

  // Now start the context and verify that certain properties changed as anticipated:
  //m_create->Initiate();
  //ASSERT_TRUE(jb->DelayUntilCanAccept()) << "AutoJob did not correctly delay for dispatch acceptance";
  //ASSERT_TRUE(jb->CanAccept()) << "AutoJob failed to correctly report that it could accept dispatch events";

  // Verify that the blocked thread has become unblocked and quits properly:
  //ASSERT_TRUE(t.try_join_for(boost::chrono::seconds(1))) << "AutoJob did not correctly signal a blocked thread that it was ready to accept dispatchers";
}

TEST_F(AutoJobTest, VerifySimpleSubmission) {
  AutoRequired<AutoJob> jb;
  
  auto myFlag = std::make_shared<bool>(false);
  *jb += [myFlag] {
    *myFlag = true;
  };

  // Kickoff, signal for a shutdown to take place, and then verify the flag
  AutoCurrentContext ctxt;
  ctxt->Initiate();
  ctxt->SignalShutdown(true);
  ASSERT_TRUE(*myFlag) << "AutoJob did not properly execute its thread";
}

TEST_F(AutoJobTest, VerifyTeardown) {
  AutoRequired<AutoJob> job;
  AutoCurrentContext ctxt;
  
  bool check1 = false;
  bool check2 = false;
  bool check3 = false;
  
  *job += [&check1] {
    boost::this_thread::sleep(boost::posix_time::milliseconds(200));
    check1 = true;
  };
  *job += [&check2] {
    boost::this_thread::sleep(boost::posix_time::milliseconds(200));
    check2 = true;
  };
  ctxt->Initiate();
  *job += [&check3] {
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    check3 = true;
  };
  
  ctxt->SignalShutdown(true);
  EXPECT_TRUE(check1) << "Lambda 1 didn't finish";
  EXPECT_TRUE(check2) << "Lambda 2 didn't finish";
  EXPECT_TRUE(check3) << "Lambda 3 didn't finish";
}
