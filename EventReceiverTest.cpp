// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "EventReceiverTest.h"
#include "Autowiring/Autowired.h"
#include "Autowiring/CoreThread.h"
#include <boost/thread/barrier.hpp>
#include <stdexcept>
#include <vector>

using namespace std;

class invalid_copycounter_exception:
  public runtime_error
{
public:
  invalid_copycounter_exception(void):
    runtime_error("Copy counter was moved, and incorrectly reused")
  {}
};

class CopyCounter {
public:
  CopyCounter(void):
    m_count(0)
  {}

  CopyCounter(const CopyCounter& rhs):
    m_count(rhs.m_count + 1)
  {
    rhs.Check();
  }

  /// <summary>
  /// Special case move ctor, which won't increment the count
  /// </summary>
  CopyCounter(CopyCounter&& rhs):
    m_count(rhs.m_count)
  {
    rhs.Check();
    rhs.m_count = -1;
  }

  /// <summary>
  /// Destructor, which invalidates the count in a specific way to indicate use-after-free
  /// </summary>
  ~CopyCounter(void) {
    m_count = -2;
  }

  // The actual count:
  int m_count;

  // Throws an exception if this copycounter is invalid
  void Check(void) const {
    if(m_count < 0)
      throw invalid_copycounter_exception();
  }

  // Copy and move overrides
  CopyCounter& operator=(const CopyCounter& rhs) {
    rhs.Check();
    m_count = rhs.m_count + 1;
    return *this;
  }

  CopyCounter& operator=(CopyCounter&& rhs) {
    rhs.Check();
    m_count = rhs.m_count;
    rhs.m_count = -1;
    return *this;
  }
};

class NoCopyClass
{
public:
  NoCopyClass():
    m_value(100)
  {}

  explicit NoCopyClass(NoCopyClass& rhs):
    m_value(101)
  {}

  int m_value;
};

class CallableInterface:
  public virtual EventReceiver
{
public:
  virtual void ZeroArgs(void) = 0;
  virtual void OneArg(int arg) = 0;
  virtual void CopyVectorForwarded(vector<int>&& vec) = 0;
  virtual void AllDone(void) = 0;

  virtual void NoCopyMethod(NoCopyClass& noCopy) {}
};

class CallableInterfaceDeferred:
  public virtual EventReceiver
{
public:
  virtual Deferred CopyVectorDeferred(const vector<int>& vec) = 0;
  virtual Deferred ZeroArgsDeferred(void) = 0;
  virtual Deferred OneArgDeferred(int arg) = 0;
  virtual Deferred TrackCopy(CopyCounter&& ctr) = 0;
  virtual Deferred AllDoneDeferred(void) = 0;
};

class Jammer:
  public CoreThread
{
public:
  Jammer(void):
    totalXmit(0)
  {
    Ready();
  }

  volatile int totalXmit;

  AutoFired<CallableInterface> m_ci;

  void Run(void) override {
    while(!ShouldStop() && totalXmit < 0x7FFFF000)
      // Jam for awhile in an asynchronous way:
      while(++totalXmit % 100)
        m_ci(&CallableInterface::ZeroArgs)();
  }
};

class SimpleReceiver:
  public CoreThread,
  public CallableInterface,
  public CallableInterfaceDeferred
{
public:
  SimpleReceiver(void):
    CoreThread("SimpleReceiver"),
    m_zero(false),
    m_one(false),
    m_oneArg(0),
    m_barrierDone(false)
  {
    Ready();
  }

private:
  // Continuity signal:
  boost::condition_variable m_continueCond;
  bool m_barrierDone;

public:
  // Manifest of functions called:
  bool m_zero;

  bool m_one;
  int m_oneArg;

  // Copy operation fields:
  vector<int> m_myVec;
  CopyCounter m_myCtr;

  ///
  // Interface utility methods:
  ///
  void ZeroArgs(void) override {
    m_zero = true;
  }

  void OneArg(int arg) override {
    m_one = true;
    m_oneArg = arg;
  }

  virtual Deferred ZeroArgsDeferred(void) override {
    m_zero = true;
    return Deferred(this);
  }

  virtual Deferred OneArgDeferred(int arg) override {
    m_one = true;
    m_oneArg = arg;
    return Deferred(this);
  }

  Deferred CopyVectorDeferred(const vector<int>& vec) override {
    // Copy out the argument:
    m_myVec = vec;
    return Deferred(this);
  }

  Deferred TrackCopy(CopyCounter&& ctr) override {
    m_myCtr = std::forward<CopyCounter>(ctr);
    return Deferred(this);
  }

  void CopyVectorForwarded(vector<int>&& vec) override {
    // Copy out the argument:
    m_myVec = vec;
  }

  void NoCopyMethod(NoCopyClass& noCopy) override {
  }

  // Trivial shutdown override
  void AllDone(void) override {
    Stop();
  }

  Deferred AllDoneDeferred(void) override {
    Stop();
    return Deferred(this);
  }

  // Overridden here so we can hit the barrier if we're still waiting on it
  void OnStop() override {
    Proceed();
  }

  /// <summary>
  /// Invoked to cause Run to continue its processing
  /// </summary>
  void Proceed(void) {
    boost::lock_guard<boost::mutex> lk(m_lock);
    if(m_barrierDone)
      return;
    m_barrierDone = true;
    m_continueCond.notify_all();
  }

  ///
  // Runs the thread
  ///
  void Run(void) override {
    {
      boost::unique_lock<boost::mutex> lk(m_lock);
      m_continueCond.wait(lk, [this] () {return m_barrierDone;});
    }
    CoreThread::Run();
  }

  // Make this method public
  using CoreThread::AcceptDispatchDelivery;
  using CoreThread::RejectDispatchDelivery;
};

EventReceiverTest::EventReceiverTest(void) {
  AutoCurrentContext ctxt;

  // Start up the context:
  ctxt->InitiateCoreThreads();
}

TEST_F(EventReceiverTest, SimpleMethodCall) {
  AutoRequired<SimpleReceiver> receiver;
  AutoFired<CallableInterface> sender;

  // Try firing the event first:
  sender(&CallableInterface::ZeroArgs)();
  sender(&CallableInterface::OneArg)(100);

  // Verify that stuff happens even when the thread isn't running:
  EXPECT_TRUE(receiver->m_zero);
  EXPECT_TRUE(receiver->m_one);
  EXPECT_EQ(100, receiver->m_oneArg);

  // Unblock:
  receiver->Proceed();
}

TEST_F(EventReceiverTest, VerifyNoReceive) {
  AutoRequired<SimpleReceiver> receiver;
  AutoFired<CallableInterfaceDeferred> sender;

  // Try to defer these calls, should not be delivered anywhere:
  sender.Defer(&CallableInterfaceDeferred::ZeroArgsDeferred)();
  sender.Defer(&CallableInterfaceDeferred::OneArgDeferred)(100);

  // Unblock:
  receiver->Proceed();

  // Allow dispatch delivery and post the quit event:
  receiver->AcceptDispatchDelivery();
  sender.Defer(&CallableInterfaceDeferred::AllDoneDeferred)();

  // Wait:
  receiver->Wait();

  // Verify that no call was not made accidentally
  EXPECT_FALSE(receiver->m_zero) << "A zero-argument call was pended to a dispatcher not marked ready";
  EXPECT_FALSE(receiver->m_one) << "A single-argument call was pended to a dispatcher not marked ready";
}

TEST_F(EventReceiverTest, DeferredInvoke) {
  AutoRequired<SimpleReceiver> receiver;
  AutoFired<CallableInterfaceDeferred> sender;

  // Accept dispatch delivery:
  receiver->AcceptDispatchDelivery();

  // Deferred fire:
  sender.Defer(&CallableInterfaceDeferred::ZeroArgsDeferred)();
  sender.Defer(&CallableInterfaceDeferred::OneArgDeferred)(101);
  sender.Defer(&CallableInterfaceDeferred::AllDoneDeferred)();

  // Verify that nothing is hit yet:
  EXPECT_FALSE(receiver->m_zero) << "Zero-argument call made prematurely";
  EXPECT_FALSE(receiver->m_one) << "One-argument call made prematurely";
  EXPECT_TRUE(receiver->IsRunning()) << "Receiver is terminated";

  // Unblock:
  receiver->Proceed();

  // Now wait until all events are processed:
  receiver->Wait();

  // Validate deferred firing:
  EXPECT_TRUE(receiver->m_zero) << "Zero argument call was not properly deferred";
  EXPECT_TRUE(receiver->m_one) << "Single argument call was not properly deferred";
  EXPECT_EQ(101, receiver->m_oneArg) << "Argument was not correctly propagated through a deferred call";
}

TEST_F(EventReceiverTest, NontrivialCopy) {
  AutoRequired<SimpleReceiver> receiver;
  AutoFired<CallableInterfaceDeferred> sender;

  // Accept dispatch delivery:
  receiver->AcceptDispatchDelivery();

  static const int sc_numElems = 10;

  // Create the vector we're going to copy over:
  vector<int> ascending;
  for(int i = 0; i < sc_numElems; i++)
    ascending.push_back(i);

  // Deferred fire:
  sender.Defer(&CallableInterfaceDeferred::CopyVectorDeferred)(ascending);
  sender.Defer(&CallableInterfaceDeferred::AllDoneDeferred)();

  // Verify that nothing is hit yet:
  EXPECT_TRUE(receiver->m_myVec.empty()) << "Event handler invoked before barrier was hit; it should have been deferred";
  EXPECT_TRUE(receiver->IsRunning()) << "Receiver is terminated";

  // Unblock:
  receiver->Proceed();

  // Now wait until all events are processed:
  receiver->Wait();

  // Validate our vectors:
  ASSERT_EQ(10, (int)receiver->m_myVec.size()) << "Receiver was not populated correctly with a vector";
  for(int i = 0; i < sc_numElems; i++)
    EXPECT_EQ(i, ascending[i]) << "Element at offset " << i << " was incorrectly copied";
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
  receiver->AcceptDispatchDelivery();

  // Make our copy counter:
  CopyCounter ctr;

  // Can't test this until MSVC11!

#if _MSC_VER >= 1700
  // Pass the field in:
  sender(&CallableInterfaceDeferred::TrackCopy)(std::move(ctr));
#endif

  // Signal stop:
  sender.Defer(&CallableInterfaceDeferred::AllDoneDeferred)();

  // Let the sender process and then wait for it before we go on:
  receiver->Proceed();
  receiver->Wait();

  // There are limitations to how few times we can copy.  At a minimum, there's one
  // copy from the the Defer call, and one again when the deferred method is passed
  // the DispatchQueue
  CopyCounter& finalCtr = receiver->m_myCtr;
  EXPECT_LE(finalCtr.m_count, 2) << "Transfer object was copied too many times";
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
      CurrentContextPusher pshr(subCtxt);
      subCtxtWeak = subCtxt;

      // Create a new descendant event receiver that matches a parent context type and should
      // be autowired to grab events from the parent:
      AutoRequired<SimpleReceiver> rcvr;
      rcvrWeak = rcvr;
      rcvrCopy = rcvr;

      // Now we try to fire and verify it gets caught on the receiver side:
      sender(&CallableInterface::ZeroArgs)();

      // Verify that it gets caught:
      EXPECT_TRUE(rcvr->m_zero) << "Event receiver in descendant context was not properly autowired";
    }

    // Verify subcontext is gone:
    EXPECT_TRUE(subCtxtWeak.expired()) << "Subcontext endured outside of its intended scope";

    // Verify the reference count on the event receiver
    EXPECT_EQ(1, rcvrCopy.use_count()) << "Detected a leaked reference to an event receiver";

    // Fire the event again--shouldn't be captured by the receiver because its context is gone
    rcvrCopy->m_zero = false;
    sender(&CallableInterface::ZeroArgs)();
    EXPECT_FALSE(rcvrCopy->m_zero) << "Event receiver was still wired even after its enclosing context was removed";
  }

  // The parent context had better not be holding a reference at this point
  EXPECT_TRUE(rcvrWeak.expired()) << "Event receiver reference still being held after its context and all shared references are gone";

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
  AutoRequired<Jammer> jammer;

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

  // Now we begin teardown operations.  If there is an improper event receiver, this will crash hard.
  AutoCurrentContext ctxt;
  ctxt->SignalTerminate();
}

TEST_F(EventReceiverTest, PathologicalTransmitterTest) {
  // Set up the jammer and receiver collections:
  AutoRequired<Jammer> jammer;

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
