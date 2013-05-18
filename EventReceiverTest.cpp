#include "stdafx.h"
#include "EventReceiverTest.h"
#include "Autowiring/Autowired.h"
#include "Autowiring/CoreThread.h"
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
    m_barrier(2),
    m_barrierDone(false)
  {
    Ready();
  }

private:
  // Continuity barrier, used to pause processing while in Run
  boost::barrier m_barrier;
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

  void CopyVector(const vector<int>& vec) override {
    // Copy out the argument:
    m_myVec = vec;
  }

  void CopyVectorForwarded(vector<int>&& vec) override {
    // Copy out the argument:
    m_myVec = vec;
  }

  void TrackCopy(CopyCounter&& ctr) override {
    m_myCtr = std::forward<CopyCounter>(ctr);
  }

  // Trivial shutdown override
  void AllDone(void) override {
    Stop();
  }

  // Overridden here so we can hit the barrier if we're still waiting on it
  void Stop() override {
    Proceed();
    CoreThread::Stop();
  }

  /// <summary>
  /// Invoked to cause Run to continue its processing
  /// </summary>
  void Proceed(void) {
    {
      boost::lock_guard<boost::mutex> lk(m_lock);
      if(m_barrierDone)
        return;
      m_barrierDone = true;
    }
    m_barrier.wait();
  }

  ///
  // Runs the thread
  ///
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
  receiver->Proceed();
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
  receiver->Proceed();

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
  receiver->Proceed();

  // Now wait until all events are processed:
  receiver->Wait();

  // Validate our vectors:
  ASSERT_EQ(10, receiver->m_myVec.size()) << "Receiver was not populated correctly with a vector";
  for(size_t i = 0; i < sc_numElems; i++)
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
  
  // Make our copy counter:
  CopyCounter ctr;

  // Pass the field in:
  sender->Defer(&CallableInterface::TrackCopy)(
    ctr
  );

  // Signal stop:
  sender->Defer(&CallableInterface::AllDone)();

  // Let the sender process and then wait for it before we go on:
  receiver->Proceed();
  receiver->Wait();
  
  // There are limitations to how few times we can copy.  At a minimum, there's one
  // copy from the the Defer call, and one again when the deferred method is passed
  // the DispatchQueue
  CopyCounter& finalCtr = receiver->m_myCtr;
  EXPECT_LE(finalCtr.m_count, 2) << "Transfer object was copied too many times";
}
