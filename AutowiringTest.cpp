#include "stdafx.h"
#include "AutowiringTest.h"
#include "Autowired.h"
#include "CoreThread.h"
#include "TestFixtures/SimpleObject.h"
#include "TestFixtures/SimpleReceiver.h"

TEST_F(AutowiringTest, VerifyAutowiredFast) {
  // Add an object:
  m_create->Inject<SimpleObject>();

  // Verify that AutowiredFast can find this object
  AutowiredFast<SimpleObject> sobj;
  ASSERT_TRUE(sobj.IsAutowired()) << "Failed to autowire an object which was just injected into a context";
}

TEST_F(AutowiringTest, VerifyAutowiredFastNontrivial) {
  // This will cause a cache entry to be inserted into the CoreContext's memoization system.
  // If there is any improper or incorrect invalidation in that system, then the null entry
  // will create problems when we attempt to perform an AutowiredFast later on.
  AutowiredFast<CallableInterface>();

  // Now we add the object
  AutoRequired<SimpleReceiver>();

  // Verify that AutowiredFast can find this object from its interface
  AutowiredFast<CallableInterface> ci;
  ASSERT_TRUE(ci.IsAutowired()) << "Failed to autowire an interface advertised by a newly-inserted object";
}

template<int N>
class AutowiresOneThingThenQuits:
  public CoreThread
{
public:
  AutoRequired<boost::mutex> m_ctxtLock;

  void Run(void) override {
    boost::lock_guard<boost::mutex> lk(*m_ctxtLock);
    Autowired<SimpleObject>();
  }
};

template<class... Args>
void noop(Args...) {}

template<int... N>
void InjectAll(seq<N...>) {
  noop(
    AutoRequired<AutowiresOneThingThenQuits<N>>()...
  );
}

TEST_F(AutowiringTest, PathologicalAutowiringRace) {
  AutoRequired<boost::mutex> lock;

  (boost::lock_guard<boost::mutex>)*lock,
  InjectAll(gen_seq<100>::type());

  // Now insert at about the same time as other threads are waking up.  If there are synchronization problems
  // in spin-up or tear-down, 
  AutoRequired<SimpleObject>();
}