#include "stdafx.h"
#include "CoreJobTest.h"
#include "CoreJob.h"
#include "move_only.h"
#include THREAD_HEADER
#include FUTURE_HEADER

TEST_F(CoreJobTest, VerifySimpleProperties) {
  AutoRequired<CoreJob> jb;

  ASSERT_FALSE(m_create->IsInitiated()) << "CoreJob reported it could receive events before its enclosing context was created";

  // Create a thread which will delay for acceptance, and then quit:
  auto future = std::async(std::launch::async, [this] { //GRAHAM
    m_create->DelayUntilInitiated();
  });

  // Verify that this thread doesn't back out right away:
  ASSERT_EQ(std::future_status::timeout, future.wait_for(std::chrono::milliseconds(10))) << "CoreJob did not block a client who was waiting for its readiness to accept dispatchers";

  // Now start the context and verify that certain properties changed as anticipated:
  m_create->Initiate();
  ASSERT_TRUE(m_create->DelayUntilInitiated()) << "CoreJob did not correctly delay for dispatch acceptance";

  // Verify that the blocked thread has become unblocked and quits properly:
  ASSERT_EQ(std::future_status::ready, future.wait_for(std::chrono::seconds(1))) << "CoreJob did not correctly signal a blocked thread that it was ready to accept dispatchers";
}

TEST_F(CoreJobTest, VerifySimpleSubmission) {
  AutoRequired<CoreJob> jb;

  auto myFlag = std::make_shared<bool>(false);
  *jb += [myFlag] {
    *myFlag = true;
  };

  // Kickoff, signal for a shutdown to take place, and then verify the flag
  AutoCurrentContext ctxt;
  ctxt->Initiate();
  ctxt->SignalShutdown(true);
  ASSERT_TRUE(*myFlag) << "CoreJob did not properly execute its thread";
}

TEST_F(CoreJobTest, VerifyTeardown) {
  AutoRequired<CoreJob> job;
  AutoCurrentContext ctxt;

  bool check1 = false;
  bool check2 = false;
  bool check3 = false;

  *job += [&check1] {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    check1 = true;
  };
  *job += [&check2] {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    check2 = true;
  };
  ctxt->Initiate();
  *job += [&check3] {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    check3 = true;
  };

  ctxt->SignalShutdown(true);
  EXPECT_TRUE(check1) << "Lambda 1 didn't finish";
  EXPECT_TRUE(check2) << "Lambda 2 didn't finish";
  EXPECT_TRUE(check3) << "Lambda 3 didn't finish";
}

struct SimpleListen:
  virtual EventReceiver
{
  SimpleListen():
    m_flag(false)
  {}

  void SetFlag(){m_flag=true;}

  bool m_flag;
};

TEST_F(CoreJobTest, VerifyNoEventReceivers){
  AutoCreateContext ctxt1;
  CurrentContextPusher pshr1(ctxt1);

  AutoFired<SimpleListen> fire;
  ctxt1->Initiate();

  AutoCreateContext ctxt2;
  CurrentContextPusher pshr2(ctxt2);

  AutoRequired<SimpleListen> listener;
  ASSERT_FALSE(listener->m_flag) << "Flag was initialized improperly";

  fire(&SimpleListen::SetFlag)();
  EXPECT_FALSE(listener->m_flag) << "Lister recived event event though it wasn't initiated";
}

class CanOnlyMove {
public:
  CanOnlyMove(){}
  ~CanOnlyMove(){}
  CanOnlyMove(const CanOnlyMove& derp) = delete;
  CanOnlyMove(CanOnlyMove&& derp){}
};

TEST_F(CoreJobTest, MoveOnly){
  CanOnlyMove move;
  //CanOnlyMove derp = move; //error

  MoveOnly<CanOnlyMove> mo(std::move(move));

  MoveOnly<CanOnlyMove> first = mo;
  //MoveOnly<CanOnlyMove> second = mo; //error
}

TEST_F(CoreJobTest, AbandonedDispatchers) {
  auto v = std::make_shared<bool>(false);

  AutoRequired<CoreJob> cj;
  *cj += [v] { *v = true; };

  // Graceful shutdown on our enclosing context without starting it:
  AutoCurrentContext()->SignalShutdown(true);

  // Verify that all lambdas on the CoreThread got called as expected:
  ASSERT_FALSE(*v) << "Lambdas attached to a CoreJob should not be executed when the enclosing context is terminated without being started";
}

TEST_F(CoreJobTest, RecursiveAdd) {
  bool first = false;
  bool second = false;
  bool third = false;
  
  AutoRequired<CoreJob> cj;
  
  AutoCurrentContext()->Initiate();
  
  *cj += [&first,&second,&third, &cj] {
    first = true;
    *cj += [&first,&second,&third,&cj] {
      second = true;
      *cj += [&first,&second,&third,&cj] {
        third = true;
      };
      cj->Stop(true);
    };
  };
  
  cj->Wait();
  
  // Verify that all lambdas on the CoreThread got called as expected:
  EXPECT_TRUE(first) << "Appended lambda didn't set value";
  EXPECT_TRUE(second) << "Appended lambda didn't set value";
  EXPECT_TRUE(third) << "Appended lambda didn't set value";
}

TEST_F(CoreJobTest, RaceCondition) {
  for (int i=0; i<5; i++) {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);
    AutoRequired<CoreJob> cj;
    ctxt->Initiate();
    
    bool first = false;
    bool second = false;
    
    *cj += [&first] {
      first = true;
    };
    
    std::this_thread::sleep_for(std::chrono::milliseconds(i));
    
    *cj += [&second, &cj] {
      second = true;
    };
    
    ctxt->SignalShutdown(true);
    
    ASSERT_TRUE(first) << "Failed after set value in lambda";
    ASSERT_TRUE(second) << "Failed to set value when delayed " << i << " milliseconds";
  }
}

TEST_F(CoreJobTest, CorrectlyAssignedCurrentContext) {
  AutoCurrentContext()->Initiate();
  AutoRequired<CoreJob> job;

  std::shared_ptr<CoreContext> ctxt;
  *job += [&ctxt] { ctxt = AutoCurrentContext(); };
  *job += [job] { job->Stop(true); };
  ASSERT_TRUE(job->WaitFor(std::chrono::seconds(5)));

  // Now verify that the job was run in the right thread context:
  ASSERT_EQ(AutoCurrentContext(), ctxt) << "Job lambda was not run with the correct CoreContext current";
}
