#include "stdafx.h"
#include "CoreContextTest.h"
#include <set>

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