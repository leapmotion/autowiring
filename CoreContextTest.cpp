#include "stdafx.h"
#include "CoreContextTest.h"
#include <set>

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
  m_create->EnumerateChildContexts(
    [&allChildren](std::shared_ptr<CoreContext> ctxt){
      allChildren.insert(ctxt);
    }
  );

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
  
  auto onlyFoos = m_create->EnumerateChildContexts<Foo>();
  ASSERT_EQ(2UL, onlyFoos.size()) << "Didn't collect only contexts with 'Foo' sigil";
  ASSERT_NE(std::find(onlyFoos.begin(), onlyFoos.end(), fooCtxt), onlyFoos.end()) << "Context not enumerated";
  ASSERT_NE(std::find(onlyFoos.begin(), onlyFoos.end(), childFoo), onlyFoos.end()) << "Context not enumerated";

  auto onlyBars = m_create->EnumerateChildContexts<Bar>();
  ASSERT_EQ(1UL, onlyBars.size()) << "Didn't collect only contexts with 'Bar' sigil";
  ASSERT_NE(std::find(onlyBars.begin(), onlyBars.end(), barCtxt), onlyBars.end()) << "Context not enumerated";

  auto noBaz = m_create->EnumerateChildContexts<Baz>();
  ASSERT_TRUE(noBaz.empty()) << "Incorrectly collected contexts with 'Baz' sigil";
}

TEST_F(CoreContextTest, TestEarlyLambdaReturn) {
  // Create three children:
  AutoCreateContext child1;
  AutoCreateContext child2;
  AutoCreateContext child3;

  // Enumerate, but stop after three:
  std::set<std::shared_ptr<CoreContext>> allChildren;
  size_t totalSoFar = 0;
  m_create->EnumerateChildContexts(
    [&allChildren, &totalSoFar](std::shared_ptr<CoreContext> ctxt) -> bool {
      if(totalSoFar++ == 3)
        return false;

      allChildren.insert(ctxt);
      return true;
    }
  );

  ASSERT_EQ(3UL, allChildren.size()) << "Enumeration routine failed to quit early";

  // Verify that the root context isn't in the set--needed to assure that we are running a depth-first search.
  ASSERT_EQ(0UL, allChildren.count(m_create)) << "EnumerateChildContexts did not execute depth-first";
}

TEST_F(CoreContextTest, ChildEnumerationIsCorrect) {
  // Create a few child contexts:
  AutoCreateContext v, w;

  // Shouldn't find any contexts at this point:
  auto vec = AutoCurrentContext()->EnumerateChildContexts<Object>();
  ASSERT_TRUE(vec.empty()) << "Sigil-restricted child enumeration incorrectly detected at least one sigil-bearing child context";

  // Create one sigil-bearing context:
  AutoCreateContextT<Object> sigilContext;

  // Verify that we only enumerate one child context:
  vec = AutoCurrentContext()->EnumerateChildContexts<Object>();
  ASSERT_EQ(1UL, vec.size()) << "Sigil-restricted child enumeration returned too many contexts";
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
      throw std::exception("");
  }

  static unsigned char s_space[];
  static size_t s_deleterHitCount;

  static void* operator new(size_t){
    return s_space;
  }
  
  static void operator delete(void*) {
    s_deleterHitCount++;
  }
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