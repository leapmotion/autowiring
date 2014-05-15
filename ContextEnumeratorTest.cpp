#include "stdafx.h"
#include "ContextEnumeratorTest.h"
#include "ContextEnumerator.h"

TEST_F(ContextEnumeratorTest, VerifySimpleEnumeration) {
  // Create a pair of descendant contexts, verify we don't accidentally hit these when enumerating children
  AutoCreateContext outer;
  AutoCreateContext inner;
  CurrentContextPusher pshr(inner);

  // Add a few children to the current context:
  AutoCreateContext c1;
  AutoCreateContext c2;
  AutoCreateContext c3;
  AutoCreateContext c4;

  std::unordered_set<std::shared_ptr<CoreContext>> allCtxts;
  allCtxts.insert(AutoCurrentContext());
  allCtxts.insert(c1);
  allCtxts.insert(c2);
  allCtxts.insert(c3);
  allCtxts.insert(c4);

  // Create the enumerator, verify we get the expected count:
  size_t nContexts = 0;
  for(const std::shared_ptr<CoreContext>& cur : CurrentContextEnumerator()) {
    ASSERT_TRUE(!!allCtxts.count(cur)) << "Context enumerator accidentally enumerated a context not rooted in the specified parent";

    allCtxts.erase(cur);
    nContexts++;
  }

  ASSERT_EQ(5UL, nContexts) << "Context enumerator did not encounter the number of expected children";
  ASSERT_TRUE(allCtxts.empty()) << "Context enumerator did not encounter all children as expected";
}