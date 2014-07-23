// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "PostConstructTest.h"
#include "Autowired.h"
#include "ContextMember.h"
#include "TestFixtures/SimpleObject.h"
#include THREAD_HEADER

using namespace std;

class ContextExposer:
  public CoreContext
{
public:
  size_t DeferredCount(void) const {
    size_t ct = 0;
    for(auto& entry : CoreContext::m_typeMemos)
      for(auto cur = entry.second.pFirst; cur; cur = cur->GetFlink())
        ct++;
    return ct;
  }
};

// Two classes to make up the cyclic dependency:
class A:
  public ContextMember
{
public:
  A(void):
    m_value(2)
  {}

  int m_value;

  int GetValue(void) const {return m_value;}
};

// For testing NotifyWhenAutowired with heirarchies
class Interface
{
public:
  virtual int GetValue() = 0;
};

class Implementation :
  public ContextMember,
  public Interface
{
public:
  virtual int GetValue() override { return 2; }
};

class Naive:
  public ContextMember
{
public:
  Naive(void) {
    if(!m_a)
      throw exception();
  }

  Autowired<A> m_a;
};

class Smarter:
  public ContextMember
{
public:
  Smarter(void):
    value(1)
  {
    m_a.NotifyWhenAutowired([this] () {
      this->value = m_a->GetValue();
    });
  }

  int value;
  Autowired<A> m_a;
};

class SmarterInterface
{
public:
  SmarterInterface(void):
    value(1)
  {
    m_interface.NotifyWhenAutowired([this] () {
      this->value = m_interface->GetValue();
    });
  }

  int value;
  Autowired<Interface> m_interface;
};

class FailedAutowiringInstance {
};

TEST_F(PostConstructTest, VerifyNaiveBehavior) {
  // Create a context and add just the naive class, to verify the problematic behavior:
  AutoCurrentContext ctxt;
  EXPECT_THROW(ctxt->Inject<Naive>(), std::exception) << "Naive class didn't throw an exception as expected";
}

TEST_F(PostConstructTest, VerifyExpectedDeferrmentCount) {
  AutoCurrentContext ctxt;

  // Add the smart class, which should introduce a single deferred count:
  ctxt->Inject<Smarter>();

  // Now test the count:
  EXPECT_EQ(
    1UL,
    ((ContextExposer&)*ctxt).DeferredCount()
  ) << "Unexpected number of deferred initializers";
}

TEST_F(PostConstructTest, VerifySmartBehavior) {
  AutoCurrentContext ctxt;

  // Add the smart class, which has a member that autowired type A
  ctxt->Inject<Smarter>();

  // Check that we can get the item we just injected
  Autowired<Smarter> smarter;
  ASSERT_TRUE(smarter.IsAutowired()) << "Slot was not satisfied as expected";
  EXPECT_EQ(1, smarter->value) << "Unexpected initial value of SmarterA instance";

  // Now inject A, and see if delayed autowiring has taken place:
  ctxt->Inject<A>();
  EXPECT_FALSE(!smarter->m_a.get()) << "Autowired member was not wired as expected";

  // Verify the value was updated by the notification routine
  EXPECT_EQ(2, smarter->value) << "Post-construction notification routine wasn't invoked as expected";
}

TEST_F(PostConstructTest, VerifySmartBehaviorWithInheritance) {
  AutoCurrentContext ctxt;

  // Add the smart classes, which should succeed
  ctxt->Inject<SmarterInterface>();

  //Initially value should be one, which is the default
  Autowired<SmarterInterface> smarterI;
  EXPECT_EQ(1, smarterI->value) << "Unexpected initial value of SmarterA instance";

  //Now add Implementation and check the wiring
  ctxt->Inject<Implementation>();
  EXPECT_FALSE(!smarterI->m_interface.get()) << "Autowired subclass was not wired as expected";

  EXPECT_EQ(2, smarterI->value) << "Post-construction notification routine wasn't invoked on subclass";
}

TEST_F(PostConstructTest, VerifyLoopingFailedAutowiring) {
  for(size_t i = 10; i--;) {
    Autowired<FailedAutowiringInstance> ignored;
    ASSERT_FALSE(ignored.IsAutowired()) << "Successfully autowired an instance that should not have been autowirable";
  }
}

TEST_F(PostConstructTest, VerifyTrivialNotifyWhenAutowired) {
  // Inject a type first:
  AutoRequired<SimpleObject>();

  // Now autowire, and add a registration:
  bool called = false;
  Autowired<SimpleObject> so;
  so.NotifyWhenAutowired([&called] { called = true; });

  ASSERT_TRUE(called) << "An autowiring notification was not invoked on an already-satisfied field as expected";
}

TEST_F(PostConstructTest, MultiNotifyWhenAutowired) {
  // Add multiple notifications on the same space:
  int field = 0;
  Autowired<SimpleObject> so;
  for(size_t i = 10; i--;)
    so.NotifyWhenAutowired([&field] { field++; });

  // Inject the type to trigger the autowiring
  AutoRequired<SimpleObject>();

  // Verify that the notification got hit ten times:
  ASSERT_EQ(10, field) << "Autowiring lambdas did not run the expected number of times";
}

TEST_F(PostConstructTest, NotificationTeardownRace) {
  std::shared_ptr<CoreContext> pContext;

  auto quit = false;
  auto shouldQuit = MakeAtExit([&quit] { quit = true; });
  std::atomic<size_t> counter{0};

  // This thread sets up the race pathology:
  std::thread t([&] {
    while(!quit) {
      // Barrier until setup time:
      while(counter != 1)
        std::this_thread::yield();

      if(!pContext)
        break;

      // Set the context current, then try to autowire:
      CurrentContextPusher pshr(pContext);
      Autowired<SimpleObject> sobj;
      sobj.NotifyWhenAutowired([] {});

      // Now barrier, and then we will try to race against the context
      // for teardown.
      counter++;
    }
  });

  for(size_t i = 0; i < 200; i++) {
    // Make a new context:
    AutoCreateContext ctxt;
    pContext = ctxt;

    // Wake up the other thread, let it set a notify-when-autowired:
    counter++;
    while(counter != 2)
      std::this_thread::yield();

    // Counter goes back to zero before we make the next loop iteration:
    counter = 0;

    // Now we reset our pContext pointer, and then tell the thread
    // to race with us against context teardown:
    pContext.reset();
  }

  // All done, wait for the thread to back out:
  quit = true;
  counter = 1;
  t.join();
}

TEST_F(PostConstructTest, VerifyAllInstancesSatisfied) {
  // Create all of our slots and bind to them:
  const size_t ct = 3;
  Autowired<SimpleObject> aw[ct];
  bool hit[ct] = {};

  for(size_t i = ct; i--;)
    aw[i].NotifyWhenAutowired([&hit, i] { hit[i] = true; });

  // Now we inject our simple object:
  AutoRequired<SimpleObject>();

  // Verify that everything got hit:
  for(size_t i = 0; i < ct; i++) {
    ASSERT_TRUE(aw[i]) << "Autowired slot " << i << " was not post-bound correctly";
    ASSERT_TRUE(hit[i]) << "Autowired slot " << i << " did not fire all of its post-construct notifiers as expected";
  }
}

TEST_F(PostConstructTest, ContextNotifyWhenAutowired) {
  auto called = std::make_shared<bool>(false);
  
  // Now we'd like to be notified when SimpleObject gets added:
  m_create->NotifyWhenAutowired<SimpleObject>(
    [called] {
      *called = true;
    }
  );

  // Should only be two uses, at this point, of the capture of the above lambda:
  EXPECT_EQ(2L, called.use_count()) << "Unexpected number of references held in a capture lambda";

  // Create another entry that will add another slot to the deferred list:
  Autowired<SimpleObject> sobj;

  // Insert the SimpleObject, see if the lambda got hit:
  AutoRequired<SimpleObject>();
  ASSERT_TRUE(*called) << "Context-wide autowiring notification was not hit as expected when a matching type was injected into a context";

  // Our shared pointer should be unique by this point, because the lambda should have been destroyed
  ASSERT_TRUE(called.unique()) << "Autowiring notification lambda was not properly cleaned up";
}

