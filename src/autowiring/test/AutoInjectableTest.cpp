// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/SimpleObject.hpp"
#include <autowiring/AutoInjectable.h>

class AutoInjectableTest:
  public testing::Test
{};


TEST_F(AutoInjectableTest, BaseInjectableCase) {
  AutoCurrentContext ctxt;
  const size_t initial = ctxt->GetMemberCount();

  // Make a null injectable, and inject it
  auto base = MakeInjectable<>();
  base();

  // Verify that the context membership count did not change
  ASSERT_EQ(initial, ctxt->GetMemberCount()) << "Null injectable unexpectedly altered the context member count--it must have injected something";
}

TEST_F(AutoInjectableTest, VerifySimpleInjection) {
  auto injector = MakeInjectable<SimpleObject>();
  injector();

  Autowired<SimpleObject> myobj;
  ASSERT_TRUE(myobj.IsAutowired()) << "Injectable failed to introduce a zero-arguments constructed item into the context";
}

TEST_F(AutoInjectableTest, VerifyNullInjection) {
  AutoCurrentContext ctxt;
  size_t currentCount = ctxt->GetMemberCount();

  auto injector = MakeInjectable<void>();
  injector();

  ASSERT_EQ(currentCount, ctxt->GetMemberCount()) << "A void injector incorrectly added a context member";
}

class StealsConstructorArgument {
public:
  StealsConstructorArgument(std::string&& myVal) :
    myVal(std::forward<std::string>(myVal))
  {}

  std::string myVal;
};

TEST_F(AutoInjectableTest, VerifyConstructedInjection) {
  auto injector = MakeInjectable<StealsConstructorArgument>("Hello World!");
  injector();

  Autowired<StealsConstructorArgument> myobj;
  ASSERT_TRUE(myobj.IsAutowired()) << "Injectable failed to introduce a single-arguments constructed item into the context";
  ASSERT_EQ("Hello World!", myobj->myVal) << "Injectable failed to copy an rvalue argument properly";
}

TEST_F(AutoInjectableTest, VerifyReduplicatedInjection) {
  auto injector = MakeInjectable<StealsConstructorArgument>("Hello") + MakeInjectable<StealsConstructorArgument>("World");
  injector();

  Autowired<StealsConstructorArgument> myobj;
  ASSERT_TRUE(myobj.IsAutowired()) << "Injectable failed to introduce a single-arguments constructed item into the context";
  ASSERT_EQ("Hello", myobj->myVal) << "Injectable failed to copy an rvalue argument properly";
}

TEST_F(AutoInjectableTest, VerifyCombinedInjection) {
  auto injector = MakeInjectable<StealsConstructorArgument>("Hello") + MakeInjectable<SimpleObject>();
  injector();

  Autowired<StealsConstructorArgument> myobj;
  Autowired<SimpleObject> mySimpleObj;
  ASSERT_TRUE(myobj.IsAutowired()) << "Combined injectable failed to introduce a single-arguments constructed item into the context";
  ASSERT_EQ("Hello", myobj->myVal) << "Combined injectable failed to copy an rvalue argument properly";
  ASSERT_TRUE(mySimpleObj.IsAutowired()) << "Combined injectable failed to introduce a zero-arguments constructed item into the context";
}

TEST_F(AutoInjectableTest, VerifyInjectableAdditionPermutation1) {
  auto injector = MakeInjectable<StealsConstructorArgument>("Hello");
  auto injector2 = injector + MakeInjectable<SimpleObject>();
  injector2();

  Autowired<StealsConstructorArgument> myStealObj;
  Autowired<SimpleObject> mySimpleObj;
  ASSERT_TRUE(myStealObj.IsAutowired()) << "Combined injectable failed to introduce a single-argument constructed item";
  ASSERT_EQ("Hello", myStealObj->myVal) << "Combined injectable failed to copy an rvalue argument";
  ASSERT_TRUE(mySimpleObj.IsAutowired()) << "Combined injectable failed to introduce a zero-arguments constructed";
}


TEST_F(AutoInjectableTest, VerifyInjectableAdditionPermutation2) {
  auto injector = MakeInjectable<StealsConstructorArgument>("Hello");
  auto injector2 = MakeInjectable<SimpleObject>() + injector;
  injector2();

  Autowired<StealsConstructorArgument> myStealObj;
  Autowired<SimpleObject> mySimpleObj;
  ASSERT_TRUE(myStealObj.IsAutowired()) << "Combined injectable failed to introduce a single-argument constructed item";
  ASSERT_EQ("Hello", myStealObj->myVal) << "Combined injectable failed to copy an rvalue argument";
  ASSERT_TRUE(mySimpleObj.IsAutowired()) << "Combined injectable failed to introduce a zero-arguments constructed";
}

TEST_F(AutoInjectableTest, VerifyInjectableAdditionPermutation3) {
  auto injector = MakeInjectable<StealsConstructorArgument>("Hello");
  auto injector2 = MakeInjectable<SimpleObject>();
  auto injector3 = injector + injector2;
  injector3();

  Autowired<StealsConstructorArgument> myStealObj;
  Autowired<SimpleObject> mySimpleObj;
  ASSERT_TRUE(myStealObj.IsAutowired()) << "Combined injectable failed to introduce a single-argument constructed item";
  ASSERT_EQ("Hello", myStealObj->myVal) << "Combined injectable failed to copy an rvalue argument";
  ASSERT_TRUE(mySimpleObj.IsAutowired()) << "Combined injectable failed to introduce a zero-arguments constructed";
}

class DummyObj:
  public ContextMember
{};

TEST_F(AutoInjectableTest, VerifySimpleThreadWait) {
  // Immediate kickoff:
  AutoCurrentContext()->Initiate();

  // Make an injectable, run it, and stuff it right into a future:
  AutoFuture future;
  MakeInjectable<DummyObj, CoreThread>()(&future);

  // Make a thread and then start it going:
  Autowired<CoreThread> thread;
  ASSERT_TRUE(thread.IsAutowired()) << "Thread was not injected by an injector as expected";

  AutoRequired<std::mutex> barr;
  {
    std::lock_guard<std::mutex> lk(*barr);

    // Add a lambda that we intentionally block:
    *thread += [] {
      Autowired<CoreThread> thread;
      Autowired<std::mutex> barr;
      ASSERT_TRUE(thread && barr) << "Failed to find a required type in the current context";
      std::lock_guard<std::mutex> lk(*barr);
      thread->Stop();
    };

    // Instant wait:
    ASSERT_FALSE(future.WaitFor(std::chrono::nanoseconds(1))) << "Premature wait return on an injector-provided future";
  }

  // Now that the thread is unblocked, verify that it quits:
  ASSERT_TRUE(future.WaitFor(std::chrono::seconds(5))) << "Wait failed to return on an injector-provided future";
}

TEST_F(AutoInjectableTest, VerifyFunctionalInjectable) {
  bool val = false;
  AutoInjectable inj = MakeInjectableFn([&val] { val = true; });
  inj();

  ASSERT_TRUE(val) << "Functional injectable did not operate as expected";
}
