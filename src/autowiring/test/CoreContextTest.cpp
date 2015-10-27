// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/SimpleObject.hpp"
#include <autowiring/ContextEnumerator.h>
#include <algorithm>
#include <set>
#include THREAD_HEADER
#include FUTURE_HEADER

// A lot of clients invoke arbitrary CoreContext members during teardown of this type.  If any of those members are
// potentially abstract, the result could be a disastrous pure virtual function call.  Broadly speaking, while there
// are not guarantees that calls to the base versions of CoreContext virtual functions will perform the desired
// operation, there should never be a circumstance where such calls trigger a crash.
static_assert(!std::is_abstract<CoreContext>::value, "CoreContext cannot be abstract");

class CoreContextTest:
  public testing::Test
{};

class Foo{};
class Bar{};
class Baz{};

TEST_F(CoreContextTest, TestEnumerateChildren) {
  AutoCurrentContext ctxt;

  // Create a few anonymous children:
  AutoCreateContext child1;
  AutoCreateContext child2;
  AutoCreateContext child3;

  // Enumerate and see what we get back:
  std::set<std::shared_ptr<CoreContext>> allChildren;
  for(const auto& cur : CurrentContextEnumerator())
    allChildren.insert(cur);

  // Verify we get exactly four back:
  ASSERT_EQ(4UL, allChildren.size()) << "Failed to enumerate the correct number of child contexts";

  // Verify full membership:
  ASSERT_EQ(1UL, allChildren.count(ctxt)) << "Failed to find the root context in the returned context collection";

  const char* childMissing = "Failed to find a child context in the set of children";
  ASSERT_EQ(1UL, allChildren.count(child1)) << childMissing;
  ASSERT_EQ(1UL, allChildren.count(child2)) << childMissing;
  ASSERT_EQ(1UL, allChildren.count(child3)) << childMissing;

  //Check if filtering by sigil works
  AutoCreateContextT<Foo> fooCtxt;
  AutoCreateContextT<Bar> barCtxt;
  auto childFoo = barCtxt->Create<Foo>();

  ContextEnumeratorT<Foo> enumerator1(ctxt);
  std::vector<std::shared_ptr<CoreContext>> onlyFoos(enumerator1.begin(), enumerator1.end());
  ASSERT_EQ(2UL, onlyFoos.size()) << "Didn't collect only contexts with 'Foo' sigil";
  ASSERT_NE(std::find(onlyFoos.begin(), onlyFoos.end(), fooCtxt), onlyFoos.end()) << "Context not enumerated";
  ASSERT_NE(std::find(onlyFoos.begin(), onlyFoos.end(), childFoo), onlyFoos.end()) << "Context not enumerated";

  ContextEnumeratorT<Bar> enumerator2(ctxt);
  std::vector<std::shared_ptr<CoreContext>> onlyBars(enumerator2.begin(), enumerator2.end());
  ASSERT_EQ(1UL, onlyBars.size()) << "Didn't collect only contexts with 'Bar' sigil";
  ASSERT_NE(std::find(onlyBars.begin(), onlyBars.end(), barCtxt), onlyBars.end()) << "Context not enumerated";

  ContextEnumeratorT<Baz> enumerator3(ctxt);
  std::vector<std::shared_ptr<CoreContext>> noBaz(enumerator3.begin(), enumerator3.end());
  ASSERT_TRUE(noBaz.empty()) << "Incorrectly collected contexts with 'Baz' sigil";
}

TEST_F(CoreContextTest, TestEarlyLambdaReturn) {
  AutoCurrentContext ctxt;

  // Create three children:
  AutoCreateContext child1;
  AutoCreateContext child2;
  AutoCreateContext child3;

  // Enumerate, but stop after three:
  std::vector<std::shared_ptr<CoreContext>> allChildren;
  size_t totalSoFar = 0;
  for(const auto& ctxt : CurrentContextEnumerator()) {
    if(totalSoFar++ == 3)
      break;
    allChildren.push_back(ctxt);
  }

  ASSERT_EQ(3UL, allChildren.size()) << "Enumeration routine failed to quit early";

  // Verify that the root context is the first one enumerated--needed to assure that we are executing a depth-first search
  ASSERT_EQ(ctxt, allChildren[0]) << "EnumerateChildContexts did not execute depth-first";
}

/// <summary>
/// Be careful!  The new operator overload on this type only permits one allocation!
/// </summary>
class HasOverriddenNewOperator:
  public ContextMember
{
public:
  HasOverriddenNewOperator(bool hitDtor) {
    if(hitDtor)
      throw std::runtime_error("");
  }

  static unsigned char s_space[];
  static size_t s_deleterHitCount;

  static void* operator new(size_t) {
    return s_space;
  }
  
  static void operator delete(void*) {
    s_deleterHitCount++;
  }

  static void* operator new(size_t size, void* ptr) { return ::operator new(size, ptr); }
  static void operator delete(void* memory, void* ptr) throw() { return ::operator delete(memory, ptr); }
};

unsigned char HasOverriddenNewOperator::s_space[sizeof(HasOverriddenNewOperator)];
size_t HasOverriddenNewOperator::s_deleterHitCount;

TEST_F(CoreContextTest, CorrectHitAllocatorNew) {
  HasOverriddenNewOperator::s_deleterHitCount = 0;
  {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);

    // Verify that the overload itsef gets called as expected:
    ASSERT_EQ(
      (void*) HasOverriddenNewOperator::s_space,
      new HasOverriddenNewOperator(false)
    ) << "Overloaded new operator on a test type did not get invoked as expected";

    // Create an instance which won't throw:
    auto hono = AutoCurrentContext()->Inject<HasOverriddenNewOperator>(false);

    // Verify the correct new allocator was hit:
    ASSERT_EQ(
      (void*)HasOverriddenNewOperator::s_space,
      (void*)hono.get()
    ) << "Overridden new allocator was not invoked as anticipated";
  }

  // Verify that the deleter got hit as anticipated:
  ASSERT_EQ(1UL, HasOverriddenNewOperator::s_deleterHitCount) << "The correct deleter was not hit under ordinary teardown";
}

TEST_F(CoreContextTest, CorrectHitExceptionalTeardown) {
  HasOverriddenNewOperator::s_deleterHitCount = 0;

  // Create our type--we expect this to throw:
  ASSERT_ANY_THROW(AutoCurrentContext()->Inject<HasOverriddenNewOperator>(true)) << "Construct operation did not propagate an exception to the caller";

  // Now verify that the correct deleter was hit to release partially constructed memory:
  ASSERT_EQ(1UL, HasOverriddenNewOperator::s_deleterHitCount) << "Deleter was not correctly hit in an exceptional teardown";
}

struct NoEnumerateBeforeBoltReturn {};

class BoltThatTakesALongTimeToReturn:
  public Bolt<NoEnumerateBeforeBoltReturn>
{
public:
  void ContextCreated(void) override {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    m_bDoneRunning = true;
  }

  bool m_bDoneRunning = false;
};

TEST_F(CoreContextTest, NoEnumerateBeforeBoltReturn) {
  AutoCurrentContext ctxt;
  AutoRequired<BoltThatTakesALongTimeToReturn> longTime;

  // Spin off a thread which will create the new context
  auto finished = std::make_shared<bool>(false);
  std::thread t([finished, ctxt] {
    AutoCreateContextT<NoEnumerateBeforeBoltReturn>();
    *finished = true;
  });

  // Verify that the context does not appear until the bolt has finished running:
  while(!*finished)
    for(auto cur : ContextEnumeratorT<NoEnumerateBeforeBoltReturn>(ctxt))
      ASSERT_TRUE(longTime->m_bDoneRunning) << "A context was enumerated before a bolt finished running";

  // Need to block until this thread is done
  t.join();
}

TEST_F(CoreContextTest, InitiateCausesDelayedHold) {
  std::weak_ptr<CoreContext> ctxtWeak;

  // Create and initiate a subcontext, but do not initiate the parent context
  {
    AutoCreateContext ctxt;
    ctxtWeak = ctxt;
    ctxt->Initiate();
  }

  // Weak pointer should not be expired yet
  ASSERT_FALSE(ctxtWeak.expired()) << "Subcontext expired after initiation even though its parent context was not yet initiated";

  // Starting up the outer context should cause the inner one to self destruct
  AutoCurrentContext()->Initiate();
  ASSERT_TRUE(ctxtWeak.expired()) << "Subcontext containing no threads incorrectly persisted after termination";
}

TEST_F(CoreContextTest, InitiateOrder) {
  AutoCurrentContext testCtxt;
  testCtxt->Initiate();
  // Initiate inner to outer
  {
    auto outerCtxt = testCtxt->Create<void>();
    auto middleCtxt = outerCtxt->Create<void>();
    auto innerCtxt = middleCtxt->Create<void>();
    
    innerCtxt->Initiate();
    middleCtxt->Initiate();
    outerCtxt->Initiate();
    
    ASSERT_TRUE(outerCtxt->IsRunning()) << "Context not running after begin initiated";
    ASSERT_TRUE(middleCtxt->IsRunning()) << "Context not running after begin initiated";
    ASSERT_TRUE(innerCtxt->IsRunning()) << "Context not running after begin initiated";
    
    outerCtxt->SignalShutdown(true);
  }
  
  // Initiate outer to inner
  {
    auto outerCtxt = testCtxt->Create<void>();
    auto middleCtxt = outerCtxt->Create<void>();
    auto innerCtxt = middleCtxt->Create<void>();
    
    outerCtxt->Initiate();
    middleCtxt->Initiate();
    innerCtxt->Initiate();
    
    ASSERT_TRUE(outerCtxt->IsRunning()) << "Context not running after begin initiated";
    ASSERT_TRUE(middleCtxt->IsRunning()) << "Context not running after begin initiated";
    ASSERT_TRUE(innerCtxt->IsRunning()) << "Context not running after begin initiated";
    
    outerCtxt->SignalShutdown(true);
  }
  
  // Initiate middle, inner, then outer
  {
    auto outerCtxt = testCtxt->Create<void>();
    auto middleCtxt = outerCtxt->Create<void>();
    auto innerCtxt = middleCtxt->Create<void>();
    
    middleCtxt->Initiate();
    innerCtxt->Initiate();
    outerCtxt->Initiate();
    
    ASSERT_TRUE(outerCtxt->IsRunning()) << "Context not running after begin initiated";
    ASSERT_TRUE(middleCtxt->IsRunning()) << "Context not running after begin initiated";
    ASSERT_TRUE(innerCtxt->IsRunning()) << "Context not running after begin initiated";
    
    outerCtxt->SignalShutdown(true);
  }
}

TEST_F(CoreContextTest, InitiateMultipleChildren) {
  AutoCurrentContext testCtxt;
  testCtxt->Initiate();
  // Initiate all children
  {
    auto outerCtxt = testCtxt->Create<void>();
    auto child1 = outerCtxt->Create<void>();
    auto child2 = outerCtxt->Create<void>();
    auto child3 = outerCtxt->Create<void>();

    child1->Initiate();
    child2->Initiate();
    child3->Initiate();

    outerCtxt->Initiate();

    ASSERT_TRUE(child1->IsRunning());
    ASSERT_TRUE(child2->IsRunning());
    ASSERT_TRUE(child3->IsRunning());

    outerCtxt->SignalShutdown(true);
  }

  // Don't initiate middle child
  {
    auto outerCtxt = testCtxt->Create<void>();
    auto child1 = outerCtxt->Create<void>();
    auto child2 = outerCtxt->Create<void>();
    auto child3 = outerCtxt->Create<void>();

    child1->Initiate();
    child3->Initiate();

    outerCtxt->Initiate();

    ASSERT_TRUE(child1->IsRunning());
    ASSERT_FALSE(child2->IsInitiated());
    ASSERT_TRUE(child3->IsRunning());

    outerCtxt->SignalShutdown(true);
  }

  // Don't initiate middle child and initiate parent first
  {
    auto outerCtxt = testCtxt->Create<void>();
    auto child1 = outerCtxt->Create<void>();
    auto child2 = outerCtxt->Create<void>();
    auto child3 = outerCtxt->Create<void>();

    outerCtxt->Initiate();

    child1->Initiate();
    child3->Initiate();

    ASSERT_TRUE(child1->IsRunning());
    ASSERT_FALSE(child2->IsInitiated());
    ASSERT_TRUE(child3->IsRunning());

    outerCtxt->SignalShutdown(true);
  }
}

class CoreContextAddTestClass {};

TEST_F(CoreContextTest, CoreContextAdd) {
  auto myClass = std::make_shared<CoreContextAddTestClass>();
  AutoCurrentContext ctxt;
  ctxt->Add(myClass);

  Autowired<CoreContextAddTestClass> mc;
  ASSERT_TRUE(mc.IsAutowired()) << "Manually registered interface was not detected as expected";
}

struct ExplicitlyHoldsOutstandingCount:
  public CoreRunnable
{
public:
  bool OnStart(void) override {
    outstanding = CoreRunnable::GetOutstanding();
    return true;
  }

  void OnStop(bool) override {
    // Just mark that this event took place
    calledStop.set_value(true);
  }

  void DoAdditionalWait(void) override {
    std::unique_lock<std::mutex> lk(lock);
    cv.wait(lk, [&] { return !outstanding; });
  }

  void Proceed(void) {
    std::lock_guard<std::mutex> lk(lock);
    outstanding.reset();
    cv.notify_all();
  }

  std::promise<bool> calledStop;

  std::mutex lock;
  std::condition_variable cv;
  std::shared_ptr<CoreObject> outstanding;
};

TEST_F(CoreContextTest, AppropriateShutdownInterleave) {
  // Need both an outer and an inner context
  AutoCurrentContext ctxtOuter;
  AutoCreateContext ctxtInner;

  // Need to inject types at both scopes
  AutoRequired<ExplicitlyHoldsOutstandingCount> outer(ctxtOuter);
  AutoRequired<ExplicitlyHoldsOutstandingCount> inner(ctxtInner);

  // Start both contexts up
  ctxtOuter->Initiate();
  ctxtInner->Initiate();

  // Now shut down the outer context.  Hand off to an async, we want this to block.
  std::thread holder{
    [ctxtOuter] {
      ctxtOuter->SignalShutdown(true);
    }
  };
  auto holderClean = MakeAtExit([&holder] { holder.join(); });

  // Need to ensure that both outstanding counters are reset at some point:
  {
    auto cleanup = MakeAtExit([&] {
      outer->Proceed();
      inner->Proceed();
    });

    // Outer entry should have called "stop":
    auto future = outer->calledStop.get_future();
    ASSERT_EQ(
      std::future_status::ready,
      future.wait_for(std::chrono::seconds(5))
    ) << "Outer scope's OnStop method was incorrectly blocked by a child context member taking a long time to shut down";
  }

  // Both contexts should be stopped now:
  ASSERT_TRUE(ctxtOuter->Wait(std::chrono::seconds(5))) << "Outer context did not tear down in a timely fashion";
  ASSERT_TRUE(ctxtOuter->IsQuiescent()) << "Quiescence not achieved by outer context after shutdown";
  ASSERT_TRUE(ctxtInner->Wait(std::chrono::seconds(5))) << "Inner context did not tear down in a timely fashion";
}

class MyClassForAllBase {};

template<int N>
class MyClassForAll:
  public MyClassForAllBase
{
};

TEST_F(CoreContextTest, All) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  AutoRequired<MyClassForAll<1>> c1;
  AutoRequired<MyClassForAll<2>> c2;

  // Sanity check first:
  auto& allMembers = ctxt->All<MyClassForAllBase>();
  ASSERT_LE(2UL, allMembers.size()) << "All members not correctly identified by call to all";

  // Check all instances found
  bool found1 = false;
  bool found2 = false;
  for (auto& cur : allMembers) {
    found1 |= &cur == c1.get();
    found2 |= &cur == c2.get();
  }

  ASSERT_TRUE(found1) << "Failed to find MyClassForAll<1> via its ContextMember interface";
  ASSERT_TRUE(found2) << "Failed to find MyClassForAll<2> via its ContextMember interface";
}

class ClassThatPoints1;
class ClassThatPoints2;

class ClassThatPoints1 {
public:
  Autowired<SimpleObject> so;
  Autowired<ClassThatPoints2> b;
};
class ClassThatPoints2 {
public:
  ClassThatPoints2(const std::shared_ptr<CoreContext>& ctxt) :
    v(ctxt)
  {}

  // Contrived case, but we need to be sure that only Autowired fields pointing to objects
  // in this context are actually unlinked.  Autowired fields pointing elsewhere should be
  // left alone.
  Autowired<std::vector<int>> v;

  Autowired<SimpleObject> so;
  Autowired<ClassThatPoints1> a;
};

TEST_F(CoreContextTest, UnlinkOnTeardown) {
  std::weak_ptr<ClassThatPoints1> weakA;
  std::shared_ptr<ClassThatPoints2> strongB;
  AutoRequired<SimpleObject> so;

  // Set up a subcontext with some cycles and external links:
  {
    // Main context that gets reset second
    AutoCreateContext ctxt;

    // Sibling context that we're also going to reset
    AutoCreateContext otherContext;
    otherContext->Inject<std::vector<int>>();

    AutoRequired<ClassThatPoints1> a(ctxt);
    AutoRequired<ClassThatPoints2> b(ctxt, otherContext);
    weakA = a;
    strongB = b;
    
    ASSERT_TRUE(a->so.IsAutowired()) << "Root object pointer not correctly obtained";
    ASSERT_TRUE(b->so.IsAutowired()) << "Root object pointer not correctly obtained";

    ctxt->AddTeardownListener(
      [weakA, strongB] {
        // Verify that nothing got screwed up at this point:
        auto a = weakA.lock();
        ASSERT_FALSE(weakA.expired()) << "Weak pointer expired prematurely";
        ASSERT_EQ(strongB, a->b) << "Unlink occurred prematurely";
        ASSERT_EQ(a, strongB->a) << "Unlink occured prematurely";
      }
    );

    // Set the flag at the last possible and to ensure things still get torn down
    ctxt->SetUnlinkOnTeardown(true);
  }

  ASSERT_TRUE(weakA.expired()) << "A reference was leaked even though unlinking was turned on";
  ASSERT_TRUE(strongB->v.IsAutowired()) << "An Autowired field pointing to a foreign context was incorrectly unlinked";
  ASSERT_EQ(so.get(), strongB->so.get()) << "An Autowired field was unlinked on teardown even though it pointed outside of a context";
}

TEST_F(CoreContextTest, InitiateAssertsSignals) {
  AutoCurrentContext outer;

  auto teardown = std::make_shared<bool>(false);
  {
    AutoCreateContext ctxt;
    auto initiated = std::make_shared<bool>(false);
    auto running = std::make_shared<bool>(false);
    auto shutdown = std::make_shared<bool>(false);

    ctxt->onInitiated += [initiated] { *initiated = true; };
    ctxt->onRunning += [running] { *running = true; };
    ctxt->onShutdown += [shutdown] { *shutdown = true; };
    ctxt->onTeardown += [teardown] (const CoreContext&) { *teardown = true; };

    ctxt->Initiate();
    ASSERT_TRUE(*initiated) << "Initiation signal not asserted on context startup";
    ASSERT_FALSE(*running) << "Running signal asserted before the outer context was started";
    ASSERT_FALSE(*shutdown) << "Termination signal asserted prematurely";
    *initiated = false;

    outer->Initiate();
    ASSERT_FALSE(*initiated) << "Initiation signal was redundantly asserted";
    ASSERT_TRUE(*running) << "Running signal not asserted when the outer context was started";
    ASSERT_FALSE(*shutdown) << "Termination signal asserted prematurely";

    *running = false;

    ctxt->Initiate();
    ASSERT_FALSE(*initiated) << "Initiation signal redundantly asserted";
    ASSERT_FALSE(*running) << "Running signal redundantly asserted";
    ASSERT_FALSE(*shutdown) << "Termination signal asserted unexpectedly";

    ctxt->SignalShutdown();
    ASSERT_FALSE(*initiated) << "Initiation signal not asserted during teardown";
    ASSERT_FALSE(*running) << "Running signal asserted improperly on teardown";
    ASSERT_TRUE(*shutdown) << "Termination signal not asserted as expected";

    ASSERT_FALSE(*teardown) << "Teardown handler notified prematurely";
  }
  ASSERT_TRUE(*teardown) << "Teardown handler not correctly notified on context teardown";
}

namespace {
  class TriesToCreateChild :
    public CoreThread
  {
  public:
    TriesToCreateChild(void) :
      CoreThread("TriesToCreateChild")
    {}

    void Run(void) override {
      while (!ShouldStop());
      AutoCreateContext();
    }
  };
}

TEST_F(CoreContextTest, TerminatedContextHarmless) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();
  AutoRequired<TriesToCreateChild>{};
  ctxt->SignalShutdown();
  ASSERT_THROW(ctxt->Create<void>(), dispatch_aborted_exception) << "An exception should have been thrown when attempting to create a child from a terminated context";
}

TEST_F(CoreContextTest, Has) {
  AutoCurrentContext ctxt;
  AutoCreateContext subCtxt;
  ASSERT_FALSE(ctxt->Has<SimpleObject>()) << "Context reported as owning a type that did not yet exist";
  ASSERT_FALSE(subCtxt->Has<SimpleObject>()) << "Child context reported as owning a type that did not yet exist";
  AutoRequired<SimpleObject> so;
  ASSERT_TRUE(ctxt->Has<SimpleObject>()) << "Context failed to detect an extant type";
  ASSERT_TRUE(subCtxt->Has<SimpleObject>()) << "Child context failed to detect an extant type";
}

TEST_F(CoreContextTest, FindByTypeTest) {
  AutoCurrentContext ctxt;

  std::shared_ptr<SimpleObject> so;
  ctxt->FindByType(so);
  ASSERT_FALSE(so) << "Found a type in a context that should not exist";
}
