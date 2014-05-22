#include "stdafx.h"
#include "CoreContextTest.h"
#include "ContextEnumerator.h"
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
