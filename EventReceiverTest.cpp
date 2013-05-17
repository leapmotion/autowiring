#include "stdafx.h"
#include "EventReceiverTest.h"
#include "Autowiring/Autowired.h"
#include "Autowiring/CoreThread.h"
#include <vector>

using namespace std;

class CopyCounter {
public:
  CopyCounter(void):
    m_count(0)
  {}

  CopyCounter(const CopyCounter& rhs):
    m_count(rhs.m_count + 1)
  {}

  /// <summary>
  /// Special case move ctor, which won't increment the count
  /// </summary>
  CopyCounter(CopyCounter&& rhs):
    m_count(rhs.m_count)
  {}

  int m_count;
};

class CallableInterface:
  public EventReceiver
{
public:
  virtual void ZeroArgs(void) = 0;
  virtual void OneArg(int arg) = 0;
  virtual void CopyVector(const vector<int>& vec) = 0;
  virtual void CopyVectorForwarded(vector<int>&& vec) = 0;
  virtual void TrackCopy(CopyCounter&& ctr) = 0;
  virtual void AllDone(void) = 0;
};

class SimpleSender:
  public EventManager<CallableInterface>
{
};

class SimpleReceiver:
  public CoreThread,
  public CallableInterface
{
public:
  SimpleReceiver(void):
    CoreThread("SimpleReceiver"),
    m_zero(false),
    m_one(false),
    m_oneArg(0),
    m_barrier(2)
  {
    Ready();
  }

  // Manifest of functions called:
  bool m_zero;

  bool m_one;
  int m_oneArg;

  // Copy operation fields:
  vector<int> m_myVec;
  CopyCounter m_myCtr;

  // Continuity barrier:
  boost::barrier m_barrier;

  void ZeroArgs(void) override {
    m_zero = true;
  }

  void OneArg(int arg) override {
    m_one = true;
    m_oneArg = arg;
  }

  void CopyVector(const vector<int>& vec) override {
    // Copy out the argument:
    m_myVec = vec;
  }

  void CopyVectorForwarded(vector<int>&& vec) override {
    // Copy out the argument:
    m_myVec = vec;
  }

  void TrackCopy(CopyCounter&& ctr) override {
    m_myCtr = ctr;
  }

  void AllDone(void) override {
    Stop();
  }

  void Run(void) override {
    m_barrier.wait();
    CoreThread::Run();
  }
};

EventReceiverTest::EventReceiverTest(void) {
  AutoCurrentContext ctxt;

  // Start up the context:
  ctxt->InitiateCoreThreads();
}

TEST_F(EventReceiverTest, SimpleMethodCall) {
  // Try firing the event first:
  sender->Fire(&CallableInterface::ZeroArgs)();
  sender->Fire(&CallableInterface::OneArg)(100);

  // Verify that stuff happens even when the thread isn't running:
  EXPECT_TRUE(receiver->m_zero);
  EXPECT_TRUE(receiver->m_one);
  EXPECT_EQ(100, receiver->m_oneArg);
  
  // Unblock:
  receiver->m_barrier.wait();
}

TEST_F(EventReceiverTest, DeferredInvoke) {
  // Deferred fire:
  sender->Defer(&CallableInterface::ZeroArgs)();
  sender->Defer(&CallableInterface::OneArg)(101);
  sender->Defer(&CallableInterface::AllDone)();

  // Verify that nothing is hit yet:
  EXPECT_FALSE(receiver->m_zero) << "Zero-argument call made prematurely";
  EXPECT_FALSE(receiver->m_one) << "One-argument call made prematurely";
  EXPECT_TRUE(receiver->IsRunning()) << "Receiver is terminated";

  // Unblock:
  receiver->m_barrier.wait();

  // Now wait until all events are processed:
  receiver->Wait();

  // Validate deferred firing:
  EXPECT_TRUE(receiver->m_zero);
  EXPECT_TRUE(receiver->m_one);
  EXPECT_EQ(101, receiver->m_oneArg) << "Argument was not correctly propagated through a deferred call";
}

TEST_F(EventReceiverTest, NontrivialCopy) {
  static const size_t sc_numElems = 10;

  // Create the vector we're going to copy over:
  vector<int> ascending;
  for(size_t i = 0; i < sc_numElems; i++)
    ascending.push_back(i);

  // Deferred fire:
  sender->Defer(&CallableInterface::CopyVector)(ascending);
  sender->Defer(&CallableInterface::AllDone)();

  // Verify that nothing is hit yet:
  EXPECT_TRUE(receiver->m_myVec.empty()) << "Event handler invoked before barrier was hit; it should have been deferred";
  EXPECT_TRUE(receiver->IsRunning()) << "Receiver is terminated";

  // Unblock:
  receiver->m_barrier.wait();

  // Now wait until all events are processed:
  receiver->Wait();

  // Validate our vectors:
  ASSERT_EQ(10, receiver->m_myVec.size()) << "Receiver was not populated correctly with a vector";
  for(size_t i = 0; i < sc_numElems; i++)
    EXPECT_EQ(i, ascending[i]) << "Element at offset " << i << " was incorrectly copied";
}

TEST_F(EventReceiverTest, VerifyNoUnnecessaryCopies) {
  // Make our copy counter:
  CopyCounter ctr;

  // Verify the counter correctly tracks the number of times it was copied:
  {
    CopyCounter myCopy1 = ctr;
    ASSERT_EQ(1, myCopy1.m_count) << "Copy counter appears to be broken; cannot run test";

    CopyCounter myCopy2 = myCopy1;
    ASSERT_EQ(2, myCopy2.m_count) << "Secondary counter appears to be broken; cannot run test";

    CopyCounter myCopy3(std::move(myCopy2));
    ASSERT_EQ(2, myCopy3.m_count) << "Move ctor doesn't appear to be invoked correctly";

    // Try a move copy
    CopyCounter myCopy4;
    myCopy4 = std::move(myCopy3);
    ASSERT_EQ(2, myCopy4.m_count) << "Move assignment didn't correctly propagate the current count";
  }

  // Now we 
}
