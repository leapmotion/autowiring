#include "stdafx.h"
#include "CoreContextTest.h"
#include "AutoInjectable.h"
#include "ContextEnumerator.h"
#include <set>
#include THREAD_HEADER
#include FUTURE_HEADER

class Foo{};
class Bar{};
class Baz{};

TEST_F(CoreContextTest, TestEnumerateChildren) {
  // Create a few anonymous children:
  AutoCreateContext child1;
  AutoCreateContext child2;
  AutoCreateContext child3;

  // Enumerate and see what we get back:
  std::set<std::shared_ptr<CoreContext>> allChildren;
  for(auto cur : CurrentContextEnumerator())
    allChildren.insert(cur);

  // Verify we get exactly four back:
  ASSERT_EQ(4UL, allChildren.size()) << "Failed to enumerate the correct number of child contexts";

  // Verify full membership:
  ASSERT_EQ(1UL, allChildren.count(m_create)) << "Failed to find the root context in the returned context collection";

  const char* childMissing = "Failed to find a child context in the set of children";
  ASSERT_EQ(1UL, allChildren.count(child1)) << childMissing;
  ASSERT_EQ(1UL, allChildren.count(child2)) << childMissing;
  ASSERT_EQ(1UL, allChildren.count(child3)) << childMissing;

  //Check if filtering by sigil works
  AutoCreateContextT<Foo> fooCtxt;
  AutoCreateContextT<Bar> barCtxt;
  auto childFoo = barCtxt->Create<Foo>();

  ContextEnumeratorT<Foo> enumerator1(m_create);
  std::vector<std::shared_ptr<CoreContext>> onlyFoos(enumerator1.begin(), enumerator1.end());
  ASSERT_EQ(2UL, onlyFoos.size()) << "Didn't collect only contexts with 'Foo' sigil";
  ASSERT_NE(std::find(onlyFoos.begin(), onlyFoos.end(), fooCtxt), onlyFoos.end()) << "Context not enumerated";
  ASSERT_NE(std::find(onlyFoos.begin(), onlyFoos.end(), childFoo), onlyFoos.end()) << "Context not enumerated";

  ContextEnumeratorT<Bar> enumerator2(m_create);
  std::vector<std::shared_ptr<CoreContext>> onlyBars(enumerator2.begin(), enumerator2.end());
  ASSERT_EQ(1UL, onlyBars.size()) << "Didn't collect only contexts with 'Bar' sigil";
  ASSERT_NE(std::find(onlyBars.begin(), onlyBars.end(), barCtxt), onlyBars.end()) << "Context not enumerated";

  ContextEnumeratorT<Baz> enumerator3(m_create);
  std::vector<std::shared_ptr<CoreContext>> noBaz(enumerator3.begin(), enumerator3.end());
  ASSERT_TRUE(noBaz.empty()) << "Incorrectly collected contexts with 'Baz' sigil";
}

TEST_F(CoreContextTest, TestEarlyLambdaReturn) {
  // Create three children:
  AutoCreateContext child1;
  AutoCreateContext child2;
  AutoCreateContext child3;

  // Enumerate, but stop after three:
  std::vector<std::shared_ptr<CoreContext>> allChildren;
  size_t totalSoFar = 0;
  for(auto& ctxt : CurrentContextEnumerator()) {
    if(totalSoFar++ == 3)
      break;
    allChildren.push_back(ctxt);
  }

  ASSERT_EQ(3UL, allChildren.size()) << "Enumeration routine failed to quit early";

  // Verify that the root context is the first one enumerated--needed to assure that we are executing a depth-first search
  ASSERT_EQ(m_create, allChildren[0]) << "EnumerateChildContexts did not execute depth-first";
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
    auto hono = AutoCurrentContext()->Construct<HasOverriddenNewOperator>(false);

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
  ASSERT_ANY_THROW(AutoCurrentContext()->Construct<HasOverriddenNewOperator>(true)) << "Construct operation did not propagate an exception to the caller";

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
  auto t = std::async(std::launch::async, [ctxt] {
    AutoCreateContextT<NoEnumerateBeforeBoltReturn>();
  });

  // Verify that the context does not appear until the bolt has finished running:
  while(t.wait_for(std::chrono::milliseconds(1)) != std::future_status::ready)
    for(auto cur : ContextEnumeratorT<NoEnumerateBeforeBoltReturn>(ctxt))
      ASSERT_TRUE(longTime->m_bDoneRunning) << "A context was enumerated before a bolt finished running";
}