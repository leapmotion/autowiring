// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/SimpleObject.hpp"
#include <autowiring/Autowired.h>
#include <autowiring/ContextMember.h>
#include THREAD_HEADER

class PostConstructTest:
  public testing::Test
{};

using namespace std;

class ContextExposer:
  public CoreContext
{
public:
  size_t DeferredCount(void) const {
    size_t ct = 0;
    for(const auto& entry : CoreContext::m_typeMemos)
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
  int m_value = 2;

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
  Smarter(void)
  {
    m_a.NotifyWhenAutowired([this] () {
      this->value = m_a->GetValue();
    });
  }

  int value = 1;
  Autowired<A> m_a;
};

class SmarterInterface
{
public:
  SmarterInterface(void)
  {
    m_interface.NotifyWhenAutowired([this] () {
      this->value = m_interface->GetValue();
    });
  }

  int value = 1;
  Autowired<Interface> m_interface;
};

class FailedAutowiringInstance {
};

TEST_F(PostConstructTest, VerifyNaiveBehavior) {
  // Create a context and add just the naive class, to verify the problematic behavior:
  AutoCurrentContext ctxt;
  ASSERT_THROW(ctxt->Inject<Naive>(), std::exception) << "Naive class didn't throw an exception as expected";
}

TEST_F(PostConstructTest, VerifyExpectedDeferrmentCount) {
  AutoCurrentContext ctxt;

  // Add the smart class, which should introduce a single deferred count:
  ctxt->Inject<Smarter>();

  // Now test the count:
  ASSERT_EQ(
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
  ASSERT_EQ(1, smarter->value) << "Unexpected initial value of SmarterA instance";

  // Now inject A, and see if delayed autowiring has taken place:
  ctxt->Inject<A>();
  ASSERT_FALSE(!smarter->m_a.get()) << "Autowired member was not wired as expected";

  // Verify the value was updated by the notification routine
  ASSERT_EQ(2, smarter->value) << "Post-construction notification routine wasn't invoked as expected";
}

TEST_F(PostConstructTest, VerifySmartBehaviorWithInheritance) {
  AutoCurrentContext ctxt;

  // Add the smart classes, which should succeed
  ctxt->Inject<SmarterInterface>();

  //Initially value should be one, which is the default
  Autowired<SmarterInterface> smarterI;
  ASSERT_EQ(1, smarterI->value) << "Unexpected initial value of SmarterA instance";

  //Now add Implementation and check the wiring
  ctxt->Inject<Implementation>();
  ASSERT_FALSE(!smarterI->m_interface.get()) << "Autowired subclass was not wired as expected";

  ASSERT_EQ(2, smarterI->value) << "Post-construction notification routine wasn't invoked on subclass";
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
  AutoCurrentContext ctxt;
  
  // Now we'd like to be notified when SimpleObject gets added:
  ctxt->NotifyWhenAutowired<SimpleObject>(
    [called] {
      *called = true;
    }
  );

  // Should only be two uses, at this point, of the capture of the above lambda:
  ASSERT_EQ(2L, called.use_count()) << "Unexpected number of references held in a capture lambda";

  // Create another entry that will add another slot to the deferred list:
  Autowired<SimpleObject> sobj;

  // Insert the SimpleObject, see if the lambda got hit:
  AutoRequired<SimpleObject>();
  ASSERT_TRUE(*called) << "Context-wide autowiring notification was not hit as expected when a matching type was injected into a context";

  // Our shared pointer should be unique by this point, because the lambda should have been destroyed
  ASSERT_TRUE(called.unique()) << "Autowiring notification lambda was not properly cleaned up";
}

TEST_F(PostConstructTest, ContextNotifyWhenAutowiredPostConstruct) {
  auto called = std::make_shared<bool>(false);
  AutoCurrentContext ctxt;

  // Create an object that will satisfy subsequent notification call:
  AutoRequired<SimpleObject> sobj;

  // Notification should be immediate:
  ctxt->NotifyWhenAutowired<SimpleObject>(
    [called] {
      *called = true;
    }
  );

  // Insert the SimpleObject, see if the lambda got hit:
  ASSERT_TRUE(*called) << "Context-wide autowiring notification was not hit as expected when a matching type was injected into a context";

  // Our shared pointer should be unique by this point, because the lambda should have been destroyed
  ASSERT_TRUE(called.unique()) << "Autowiring notification lambda was not properly cleaned up";
}

class OtherObject : public SimpleObject {};

TEST_F(PostConstructTest, RecursiveNotificationPreConstruction) {
  auto called = std::make_shared<bool>(false);
  AutoCurrentContext ctxt;

  // Ensure that nested calls do not created deadlock
  // Notification should be deferred:
  ctxt->NotifyWhenAutowired<SimpleObject>(
  [called] {
    AutoCurrentContext()->NotifyWhenAutowired<OtherObject>(
    [called] {
      *called = true;
    });
  });

  // Create an object that will satisfy subsequent notification call:
  AutoRequired<SimpleObject> sobj;
  AutoRequired<OtherObject> oobj;

  // Insert the SimpleObject, see if the lambda got hit:
  ASSERT_TRUE(*called) << "Context-wide autowiring notification was not hit as expected when a matching type was injected into a context";

  // Our shared pointer should be unique by this point, because the lambda should have been destroyed
  ASSERT_TRUE(called.unique()) << "Autowiring notification lambda was not properly cleaned up";
}

TEST_F(PostConstructTest, RecursiveNotificationPostConstruction) {
  auto called = std::make_shared<bool>(false);
  AutoCurrentContext ctxt;

  // Create an object that will satisfy subsequent notification call:
  AutoRequired<SimpleObject> sobj;
  AutoRequired<OtherObject> oobj;

  // Ensure that nested calls do not created deadlock
  // Notification should be immediate:
  ctxt->NotifyWhenAutowired<SimpleObject>(
  [called] {
    AutoCurrentContext()->NotifyWhenAutowired<OtherObject>(
    [called] {
      *called = true;
    });
  });

  // Insert the SimpleObject, see if the lambda got hit:
  ASSERT_TRUE(*called) << "Context-wide autowiring notification was not hit as expected when a matching type was injected into a context";

  // Our shared pointer should be unique by this point, because the lambda should have been destroyed
  ASSERT_TRUE(called.unique()) << "Autowiring notification lambda was not properly cleaned up";
}

struct ClassWithAutoInit:
  std::enable_shared_from_this<ClassWithAutoInit>
{
  void AutoInit(void) {
    ASSERT_TRUE(m_constructed) << "A postconstruct routine was called BEFORE the corresponding constructor";
    m_postConstructed = true;

    auto myself = shared_from_this();
    ASSERT_EQ(this, myself.get()) << "Reflexive shared_from_this did not return a shared pointer to this as expected";
  }

  bool m_constructed = true;
  bool m_postConstructed = false;
};

static_assert(has_autoinit<ClassWithAutoInit>::value, "AutoInit-bearing class did not pass a static type check");

TEST_F(PostConstructTest, PostConstructGetsCalled) {
  AutoRequired<ClassWithAutoInit> cwai;
  ASSERT_TRUE(cwai->m_constructed) << "Trivial constructor call was not made as expected";
  ASSERT_TRUE(cwai->m_postConstructed) << "Auto-initialization routine was not called on an initializable type";
}

struct PostConstructThrowsException {
  void AutoInit(void) const {
    throw std::runtime_error("Autoinit crashing for no reason");
  }
};

TEST_F(PostConstructTest, PostConstructCanSafelyThrow) {
  ASSERT_ANY_THROW(AutoRequired<PostConstructThrowsException>()) << "AutoInit call threw an exception, but it was incorrectly eaten by Autowiring";

  Autowired<PostConstructThrowsException> pcte;
  ASSERT_FALSE(pcte.IsAutowired()) << "A context member which threw an exception post-construction was incorrectly introduced into a context";
}

namespace {
  class EmptyType : public CoreObject {};
}

TEST_F(PostConstructTest, StrictNotificationArrangement) {
  size_t call[3] = {0, 0, 0};
  size_t callIdx = 1;

  AutoCurrentContext ctxt;
  Autowired<EmptyType> obj;
  obj.NotifyWhenAutowired([&] { call[0] = callIdx++; });
  ctxt->NotifyWhenAutowired<EmptyType>([&] { call[1] = callIdx++; });
  obj.NotifyWhenAutowired([&] { call[2] = callIdx++; });
  ctxt->Inject<EmptyType>();

  for (size_t i = 0; i < 3; i++)
    ASSERT_EQ(i + 1, call[i]) << "Registered autowired handler was not called in the correct order";
}
