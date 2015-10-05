// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/FiresManyEventsWhenRun.hpp"
#include "TestFixtures/SimpleReceiver.hpp"
#include <autowiring/Autowired.h>
#include <autowiring/CoreThread.h>
#include <stdexcept>
#include <vector>

using namespace std;

class EventReceiverTest:
  public testing::Test
{
public:
  EventReceiverTest(void) {
    // Start up the context:
    AutoCurrentContext()->Initiate();
  }
};

TEST_F(EventReceiverTest, SimpleMethodCall) {
  AutoRequired<SimpleReceiver> receiver;
  AutoFired<CallableInterface> sender;

  // Try firing the event first:
  sender(&CallableInterface::ZeroArgs)();
  sender(&CallableInterface::OneArg)(100);

  // Verify that stuff happens even when the thread isn't running:
  ASSERT_TRUE(receiver->m_zero);
  ASSERT_TRUE(receiver->m_one);
  ASSERT_EQ(100, receiver->m_oneArg);

  // Unblock:
  receiver->Proceed();
}

TEST_F(EventReceiverTest, VerifyNoReceive) {
  AutoCreateContext ctxt;
  CurrentContextPusher pshr(ctxt);

  AutoRequired<SimpleReceiver> receiver;
  AutoFired<CallableInterfaceDeferred> sender;

  // Try to defer these calls, should not be delivered anywhere:
  ASSERT_NO_THROW(sender(&CallableInterfaceDeferred::ZeroArgsDeferred)());
  ASSERT_NO_THROW(sender(&CallableInterfaceDeferred::OneArgDeferred)(100));

  // Unblock:
  receiver->Proceed();

  // Allow dispatch delivery and post the quit event:
  //receiver->AcceptDispatchDelivery();
  AutoCurrentContext()->Initiate();
  sender(&CallableInterfaceDeferred::AllDoneDeferred)();

  // Wait:
  receiver->Wait();

  // Verify that no call was not made accidentally
  ASSERT_FALSE(receiver->m_zero) << "A zero-argument call was pended to a dispatcher not marked ready";
  ASSERT_FALSE(receiver->m_one) << "A single-argument call was pended to a dispatcher not marked ready";
}

TEST_F(EventReceiverTest, DeferredInvoke) {
  AutoRequired<SimpleReceiver> receiver;
  AutoFired<CallableInterfaceDeferred> sender;

  // Accept dispatch delivery:
  //receiver->AcceptDispatchDelivery();

  // Deferred fire:
  sender(&CallableInterfaceDeferred::ZeroArgsDeferred)();
  sender(&CallableInterfaceDeferred::OneArgDeferred)(101);
  sender(&CallableInterfaceDeferred::AllDoneDeferred)();

  // Verify that nothing is hit yet:
  ASSERT_FALSE(receiver->m_zero) << "Zero-argument call made prematurely";
  ASSERT_FALSE(receiver->m_one) << "One-argument call made prematurely";
  ASSERT_TRUE(receiver->IsRunning()) << "Receiver is terminated";

  // Unblock:
  receiver->Proceed();

  // Now wait until all events are processed:
  receiver->Wait();

  // Validate deferred firing:
  ASSERT_TRUE(receiver->m_zero) << "Zero argument call was not properly deferred";
  ASSERT_TRUE(receiver->m_one) << "Single argument call was not properly deferred";
  ASSERT_EQ(101, receiver->m_oneArg) << "Argument was not correctly propagated through a deferred call";
}

TEST_F(EventReceiverTest, NontrivialCopy) {
  AutoRequired<SimpleReceiver> receiver;
  AutoFired<CallableInterfaceDeferred> sender;

  // Accept dispatch delivery:
  //receiver->AcceptDispatchDelivery();

  static const int s_numElems = 10;

  // Create the vector we're going to copy over:
  vector<int> ascending;
  for(int i = 0; i < s_numElems; i++)
    ascending.push_back(i);

  // Deferred fire:
  sender(&CallableInterfaceDeferred::CopyVectorDeferred)(ascending);
  sender(&CallableInterfaceDeferred::AllDoneDeferred)();

  // Verify that nothing is hit yet:
  ASSERT_TRUE(receiver->m_myVec.empty()) << "Event handler invoked before barrier was hit; it should have been deferred";
  ASSERT_TRUE(receiver->IsRunning()) << "Receiver is terminated";

  // Unblock:
  receiver->Proceed();

  // Now wait until all events are processed:
  receiver->Wait();

  // Validate our vectors:
  ASSERT_EQ(10, (int)receiver->m_myVec.size()) << "Receiver was not populated correctly with a vector";
  for(int i = 0; i < s_numElems; i++)
    ASSERT_EQ(i, ascending[i]) << "Element at offset " << i << " was incorrectly copied";
}

TEST_F(EventReceiverTest, VerifyNoUnnecessaryCopies) {
  // Verify the counter correctly tracks the number of times it was copied:
  {
    CopyCounter antiRecycle;
    CopyCounter base;

    CopyCounter myCopy1 = base;
    ASSERT_EQ(1, myCopy1.m_count) << "Copy counter appears to be broken; cannot run test";

    CopyCounter myCopy2 = myCopy1;
    ASSERT_EQ(2, myCopy2.m_count) << "Secondary counter appears to be broken; cannot run test";

    // Try move construction:
    CopyCounter myCopy3(std::move(myCopy2));
    ASSERT_EQ(2, myCopy3.m_count) << "Move ctor doesn't appear to be invoked correctly";

    // Verify antirecycle:
    ASSERT_THROW(antiRecycle = myCopy2, invalid_copycounter_exception);

    // Try a move copy
    CopyCounter myCopy4;
    myCopy4 = std::move(myCopy3);
    ASSERT_EQ(2, myCopy4.m_count) << "Move assignment didn't correctly propagate the current count";
    ASSERT_THROW(antiRecycle = myCopy3, invalid_copycounter_exception);

    // Try a trivial copy:
    CopyCounter myCopy5;
    myCopy5 = myCopy4;
    ASSERT_EQ(3, myCopy5.m_count) << "Assignment operator did not increment reference count";
  }

  AutoRequired<SimpleReceiver> receiver;
  AutoFired<CallableInterfaceDeferred> sender;

  // Accept dispatch delivery:
  //receiver->AcceptDispatchDelivery();

  // Make our copy counter:
  CopyCounter ctr;

  // Signal stop:
  sender(&CallableInterfaceDeferred::AllDoneDeferred)();

  // Let the sender process and then wait for it before we go on:
  receiver->Proceed();
  receiver->Wait();

  // There are limitations to how few times we can copy.  At a minimum, there's one
  // copy from the the Defer call, and one again when the deferred method is passed
  // the DispatchQueue
  CopyCounter& finalCtr = receiver->m_myCtr;
  ASSERT_LE(finalCtr.m_count, 2) << "Transfer object was copied too many times";
}

TEST_F(EventReceiverTest, VerifyDescendantContextWiring) {
  // Sender goes in the parent context:
  AutoFired<CallableInterface> sender;

  std::weak_ptr<SimpleReceiver> rcvrWeak;
  {
    std::shared_ptr<SimpleReceiver> rcvrCopy;
    std::weak_ptr<CoreContext> subCtxtWeak;
    {
      // Create a new descendant context and put the receiver in it:
      AutoCreateContext subCtxt;
      subCtxtWeak = subCtxt;
      CurrentContextPusher pshr(subCtxt);
      subCtxt->Initiate();

      // Create a new descendant event receiver that matches a parent context type and should
      // be autowired to grab events from the parent:
      AutoRequired<SimpleReceiver> rcvr;
      rcvrWeak = rcvr;
      rcvrCopy = rcvr;

      // Now we try to fire and verify it gets caught on the receiver side:
      sender(&CallableInterface::ZeroArgs)();

      // Verify that it gets caught:
      ASSERT_TRUE(rcvr->m_zero) << "Event receiver in descendant context was not properly autowired";

      subCtxt->SignalShutdown();
      subCtxt->Wait();
      pshr.Pop();
      ASSERT_EQ(1UL, subCtxt.use_count()) << "A subcontext that should be going away was incorrectly referenced";
    }

    // Verify subcontext is gone:
    ASSERT_TRUE(subCtxtWeak.expired()) << "Subcontext endured outside of its intended scope";

    // Verify the reference count on the event receiver
    ASSERT_EQ(1, rcvrCopy.use_count()) << "Detected a leaked reference to an event receiver";

    // Fire the event again--shouldn't be captured by the receiver because its context is gone
    rcvrCopy->m_zero = false;
    sender(&CallableInterface::ZeroArgs)();
    ASSERT_FALSE(rcvrCopy->m_zero) << "Event receiver was still wired even after its enclosing context was removed";
  }

  // The parent context had better not be holding a reference at this point
  ASSERT_TRUE(rcvrWeak.expired()) << "Event receiver reference still being held after its context and all shared references are gone";

  // Fire the event again, this shouldn't cause anything to blow up!
  sender(&CallableInterface::ZeroArgs)();
}

TEST_F(EventReceiverTest, VerifyNoCopyCallable) {
  AutoFired<CallableInterface> sender;
  AutoRequired<SimpleReceiver> receiver;

  NoCopyClass method;
  sender(&CallableInterface::NoCopyMethod)(method);
}

// This instance attempts to fire an event in its dtor
class DtorFire
{
public:
  ~DtorFire(void) {
    m_fire(&CallableInterface::ZeroArgs)();
  }
  AutoFired<CallableInterface> m_fire;
};

TEST_F(EventReceiverTest, OrphanedMemberFireCheck) {
  // Create the instance and let its enclosing context go away:
  std::shared_ptr<DtorFire> dtorFireShared;
  {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);
    dtorFireShared = AutoRequired<DtorFire>();
  }

  // Verify that a trivial firing doesn't cause an exception:
  dtorFireShared->m_fire(&CallableInterface::ZeroArgs)();

  // Now release the last shared reference and ensure we can still fire
  // even in a destructor pathway:
  dtorFireShared.reset();
}

TEST_F(EventReceiverTest, PathologicalChildContextTest) {
  // Set up the jammer and receiver collections:
  AutoRequired<FiresManyEventsWhenRun> jammer;

  // This by itself is sufficient to cause problems:
  for(size_t i = 0; i < 500; i++) {
    AutoCreateContext subCtxt;
    CurrentContextPusher pshr(subCtxt);
    AutoRequired<SimpleReceiver> recvr;
  }

  // Spin until the jammer has transmitted a thousand messages:
  while(jammer->totalXmit < 1000);
  jammer->Stop();
  jammer->Wait();

  // Now we begin teardown operations. If there is an improper event receiver, this will crash hard.
  AutoCurrentContext ctxt;
  ctxt->SignalTerminate();
}

TEST_F(EventReceiverTest, PathologicalTransmitterTest) {
  // Set up the jammer and receiver collections:
  AutoRequired<FiresManyEventsWhenRun> jammer;

  for(size_t i = 0; i < 5; i++) {
    AutoCreateContext subCtxt;
    CurrentContextPusher pshr(subCtxt);
    AutoRequired<SimpleReceiver> recvr;
  }

  // Spin until the jammer has transmitted a thousand messages:
  while(jammer->totalXmit < 1000);
  jammer->Stop();
  jammer->Wait();

  // Now we begin teardown operations.  If there is an improper event receiver, this will crash hard.
  AutoCurrentContext ctxt;
  ctxt->SignalTerminate();
}

TEST_F(EventReceiverTest, VerifyDirectInvocation) {
  AutoCurrentContext ctxt;
  AutoRequired<SimpleReceiver> receiver;

  // Indirect invocation:
  ctxt->Invoke(&CallableInterface::ZeroArgs)();
  ctxt->Invoke(&CallableInterface::OneArg)(100);

  // Verify that stuff happens even when the thread isn't running:
  ASSERT_TRUE(receiver->m_zero);
  ASSERT_TRUE(receiver->m_one);
  ASSERT_EQ(100, receiver->m_oneArg);

  // Unblock:
  receiver->Proceed();
}

TEST_F(EventReceiverTest, NoEventsAfterShutdown) {
  AutoRequired<SimpleReceiver> receiver;

  // Shut down our context, and THEN fire an event
  AutoFired<CallableInterface> ci;
  AutoCurrentContext()->SignalShutdown();
  ci(&CallableInterface::ZeroArgs)();

  // Verify that the callable interface didn't get the event after shutdown
  ASSERT_FALSE(receiver->m_zero) << "A context member caught an event after its enclosing context was torn down";
}

class PassByValueInterface {
public:
  PassByValueInterface() {}
  virtual ~PassByValueInterface() {}

  virtual void ConstStringRefArg(const std::string& arg) {}
  virtual void StringArg(std::string arg) {}
};

class PassByValueReceiver:
  public CoreThread,
  public PassByValueInterface
{
public:
  PassByValueReceiver(void):
    CoreThread("PassByValueReceiver")
  {
  }
  ~PassByValueReceiver() {}

  std::string value() const {
    return m_value;
  }

private:
  std::string m_value;

public:

  ///
  /// Interface methods:
  ///
  void ConstStringRefArg(const std::string& arg) override {
    m_value = arg;
  }
  void StringArg(std::string arg) override {
    m_value = arg;
  }
};

// Create two different receivers
class PassByValueReceiver1: public PassByValueReceiver {};
class PassByValueReceiver2: public PassByValueReceiver {};

TEST_F(EventReceiverTest, VerifyMultiplePassByRef) {
  AutoFired<PassByValueInterface> sender;
  AutoRequired<PassByValueReceiver1> receiver1;
  AutoRequired<PassByValueReceiver2> receiver2;

  const std::string passByRef("pass std::string by reference");

  // Fire the "pass by ref" event:
  sender(&PassByValueInterface::ConstStringRefArg)(passByRef);
  // Verify that the value received matches what we sent both receivers:
  ASSERT_EQ(passByRef, receiver1->value());
  ASSERT_EQ(passByRef, receiver2->value());

  receiver1->Stop();
  receiver2->Stop();
  receiver1->Wait();
  receiver2->Wait();
}

TEST_F(EventReceiverTest, VerifyMultiplePassByValue) {
  AutoFired<PassByValueInterface> sender;
  AutoRequired<PassByValueReceiver1> receiver1;
  AutoRequired<PassByValueReceiver2> receiver2;

  const std::string passByValue("pass std::string by value");

  // Fire the "pass by value" event:
  sender(&PassByValueInterface::StringArg)(passByValue);

  // Verify that the value received matches what we sent both receivers:
  ASSERT_EQ(passByValue, receiver1->value());
  ASSERT_EQ(passByValue, receiver2->value());

  receiver1->Stop();
  receiver2->Stop();
  receiver1->Wait();
  receiver2->Wait();
}

TEST_F(EventReceiverTest, VerifyNoActionWhileStopped) {
  AutoCreateContext outerCtxt;
  CurrentContextPusher outerpshr(outerCtxt);

  // Firer which will operate at the outer scope:
  AutoFired<CallableInterface> ciOuter;
  AutoFired<CallableInterfaceDeferred> ciOuterDeferred;

  // Create a subcontext:
  AutoCreateContext ctxt;
  CurrentContextPusher pshr(ctxt);

  // These AutoFired will be in the interior context
  AutoFired<CallableInterface> ciInner;
  AutoFired<CallableInterfaceDeferred> ciInnerDeferred;

  // Inject a simple receiver so we can verify that it didn't catch any events:
  AutoRequired<SimpleReceiver> sr;

  ASSERT_FALSE(sr->IsRunning()) << "CoreThread was running in a context that was not started";

  // Fire events at the outer scope--this should succeed but should not be picked up by the CoreThread:
  //ASSERT_ANY_THROW(ciOuter(&CallableInterface::ZeroArgs)()) << "Firing and event before context is initiated didn't throw exception";
  ASSERT_FALSE(sr->m_zero) << "A member of an uninitialized context incorrectly received an event";

  //ciOuterDeferred(&CallableInterfaceDeferred::ZeroArgsDeferred)();
  ASSERT_EQ(0UL, sr->GetDispatchQueueLength()) << "A deferred event was incorrectly received by a member of an uninitialized context";

  // Now try to fire at the inner scope.  These fire calls MUST not be received, because firing an
  // event during context setup (say, during a constructor) is an error.

  ASSERT_TRUE(ciInner(&CallableInterface::ZeroArgs)()) << "Firing an event in a stopped context did not silently succeed";
  ciInnerDeferred(&CallableInterfaceDeferred::ZeroArgsDeferred)();
  ASSERT_FALSE(sr->m_zero) << "Fired an event in a stopped context which was incorrectly received by a member of that same context";
}

TEST_F(EventReceiverTest, VerifyCorrectContext){
  AutoCurrentContext outerCtxt;
  AutoRequired<SimpleReceiver> receiver;
  ASSERT_FALSE(receiver->m_zero) << "SimpleReceiver initialized incorrectly";
  
  AutoCreateContext innerCtxt;
  CurrentContextPusher pshr(innerCtxt);
  
  AutoFired<CallableInterface> fire(outerCtxt);
  fire(&CallableInterface::ZeroArgs)();
  ASSERT_TRUE(receiver->m_zero) << "AutoFired was created with the current context instead of the passed in context";
}

TEST_F(EventReceiverTest, EventChain){
  class Middle{
  public:
    virtual void MyEvent() = 0;
  };
  
  class MyReceiver:
    public Middle
  {
  public:
    virtual void MyEvent() {
      ASSERT_TRUE(true);
    }
  };
  
  AutoRequired<MyReceiver> recr;
  AutoCurrentContext ctxt;
  ctxt->Invoke(&MyReceiver::MyEvent)();
}

class HasAWeirdReturnType {
public:
  bool bCalled = false;

  int FiredMethod(void) {
    bCalled = true;
    return 1010;
  }
};

/// <summary>
/// Syntax verification to ensure 
TEST_F(EventReceiverTest, OddReturnTypeTest) {
  AutoRequired<HasAWeirdReturnType> hawrt;
  AutoFired<HasAWeirdReturnType> af;
  af(&HasAWeirdReturnType::FiredMethod)();

  ASSERT_TRUE(hawrt->bCalled) << "Method with a strange fired return type did not get invoked as expected";
}