// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/AutoInjectable.h>
#include <autowiring/ContextEnumerator.h>
#include <algorithm>
#include <set>
#include THREAD_HEADER
#include FUTURE_HEADER

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

struct PreBoltInjectionSigil {};

class BoltThatChecksForAThread:
  public Bolt<PreBoltInjectionSigil>
{
public:
  BoltThatChecksForAThread(void):
    m_threadPresent(false)
  {}

  void ContextCreated(void) override {
    m_threadPresent = Autowired<CoreThread>().IsAutowired();
  }

  bool m_threadPresent;
};

TEST_F(CoreContextTest, PreBoltInjection) {
  AutoRequired<BoltThatChecksForAThread> myBolt;
  AutoCreateContextT<PreBoltInjectionSigil> ctxt(MakeInjectable<CoreThread>());

  // Basic functionality check on the injectable
  Autowired<CoreThread> ct(ctxt);
  ASSERT_TRUE(ct) << "Injection did not take place in the created context as expected";

  // Verify the bolt was hit as expected
  ASSERT_TRUE(myBolt->m_threadPresent) << "Bolt was not correctly fired after injection";
}

struct NoEnumerateBeforeBoltReturn {};

class BoltThatTakesALongTimeToReturn:
  public Bolt<NoEnumerateBeforeBoltReturn>
{
public:
  BoltThatTakesALongTimeToReturn(void) :
    m_bDoneRunning(false)
  {}

  void ContextCreated(void) override {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    m_bDoneRunning = true;
  }

  bool m_bDoneRunning;
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
  {
    auto outerCtxt = testCtxt->Create<void>();
    auto middleCtxt = outerCtxt->Create<void>();
    auto innerCtxt = middleCtxt->Create<void>();
    
    middleCtxt->Initiate();
    innerCtxt->Initiate();
    outerCtxt->Initiate();
    
    EXPECT_TRUE(outerCtxt->IsRunning());
    EXPECT_TRUE(middleCtxt->IsRunning());
    EXPECT_TRUE(innerCtxt->IsRunning());
    
    outerCtxt->SignalShutdown(true);
    middleCtxt->SignalShutdown(true);
    innerCtxt->SignalShutdown(true);
  }
  
  {
    auto outerCtxt = testCtxt->Create<void>();
    auto middleCtxt = outerCtxt->Create<void>();
    auto innerCtxt = middleCtxt->Create<void>();
    
    innerCtxt->Initiate();
    middleCtxt->Initiate();
    outerCtxt->Initiate();
    
    EXPECT_TRUE(outerCtxt->IsRunning());
    EXPECT_TRUE(middleCtxt->IsRunning());
    EXPECT_TRUE(innerCtxt->IsRunning());
    
    innerCtxt->SignalShutdown(true);
    middleCtxt->SignalShutdown(true);
    outerCtxt->SignalShutdown(true);
  }
}