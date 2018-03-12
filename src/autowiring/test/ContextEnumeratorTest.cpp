// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/ContextEnumerator.h>
#include <algorithm>
#include <random>
#include MEMORY_HEADER
#include STL_UNORDERED_SET

class ContextEnumeratorTest:
  public testing::Test
{};

TEST_F(ContextEnumeratorTest, DegenerateEnumeration) {
  size_t ct = 0;
  for(const auto& cur : ContextEnumerator(std::shared_ptr<CoreContext>(nullptr))) {
    ASSERT_TRUE(!!cur) << "Context enumerator incorrectly enumerated a null context pointer";
    ct++;
  }
  ASSERT_EQ(0UL, ct) << "An empty enumerator unexpectedly enumerated one or more entries";
}

TEST_F(ContextEnumeratorTest, TrivialEnumeration) {
  size_t ct = 0;
  for(const auto& cur : ContextEnumerator(AutoCurrentContext())) {
    (void) cur;
    ct++;
  }
  ASSERT_EQ(1UL, ct) << "Context enumerator failed to enumerate a context with no children";
}

TEST_F(ContextEnumeratorTest, TwoContextEnumerationTest) {
  AutoCreateContext ctxt1;
  AutoCreateContext ctxt2;

  size_t ct = 0;
  for(const auto& cur : ContextEnumerator(ctxt1)) {
    (void) cur;
    ct++;
  }
  ASSERT_EQ(1UL, ct) << "An attempt to enumerate a context with a sibling did not correctly enumerate one context";
}

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

struct NamedContext {};

TEST_F(ContextEnumeratorTest, VerifyComplexEnumeration) {
  std::shared_ptr<CoreContext> firstNamed, secondNamed;

  AutoCreateContext firstContext;
  {
    CurrentContextPusher pshr(firstContext);
    AutoCreateContextT<NamedContext> named;
    firstNamed = named;
  }
  AutoCreateContext secondContext;
  {
    CurrentContextPusher pshr(secondContext);
    AutoCreateContextT<NamedContext> named;
    secondNamed = named;
  }

  // Verify there is only one context under the first context
  int firstCount = 0;
  for(const auto& ctxt : ContextEnumeratorT<NamedContext>(firstContext)) {
    firstCount++;
    ASSERT_EQ(ctxt, firstNamed);
    ASSERT_EQ(ctxt->GetParentContext(), firstContext);
  }
  ASSERT_EQ(firstCount, 1) << "Expected exactly one context in the parent context, found " << firstCount;

  // Verify there is only one context under the second context
  int secondCount = 0;
  for(const auto& ctxt : ContextEnumeratorT<NamedContext>(secondContext)) {
    secondCount++;
    ASSERT_EQ(ctxt, secondNamed);
    ASSERT_EQ(ctxt->GetParentContext(), secondContext);
  }
  ASSERT_EQ(secondCount, 1) << "Expected exactly one context in the parent context, found " << secondCount;

  // Verify global context structure
  auto enumerator = ContextEnumeratorT<NamedContext>(AutoGlobalContext());
  size_t globalCount = std::distance(enumerator.begin(), enumerator.end());

  ASSERT_EQ(globalCount, 2UL) << "Expected exactly one context in the parent context, found " << globalCount;
}

TEST_F(ContextEnumeratorTest, SimpleRemovalInterference) {
  static const size_t nChildren = 5;

  // Create a few contexts which we intend to destroy as we go along:
  std::unordered_set<std::shared_ptr<CoreContext>> contexts;
  for(size_t i = nChildren; i--;)
    contexts.insert(AutoCreateContext());

  // Also add ourselves:
  contexts.insert(AutoCurrentContext());

  // Enumerate contexts, and remove them from the set:
  size_t nRemoved = 0;
  for(const auto& cur : CurrentContextEnumerator()) {
    ASSERT_TRUE(!!contexts.count(cur)) << "Failed to find a context enumerated by the context enumerator";
    contexts.erase(cur);
    nRemoved++;
  }

  // Verify we got the number removed we expected:
  ASSERT_EQ(nChildren + 1UL, nRemoved) << "Context enumerator did not remove the expected number of children";
}

TEST_F(ContextEnumeratorTest, ComplexRemovalInterference) {
  static const size_t nChildren = 50;

  // This time we use a vector, and we pop from the back of the vector unpredictably:
  std::vector<std::shared_ptr<CoreContext>> children(nChildren);
  for(size_t i = nChildren; i--;)
    children.push_back(AutoCreateContext());

  // Shuffle the collection to prevent the order here from being equivalent to the order in the context
  std::random_device rd;
  std::mt19937 gen(rd());
  std::shuffle(children.begin(), children.end(), gen);

  // These are the elements actually encountered in the enumeration, held here to prevent expiration in the
  // event that we enumerate a context which should have already been evicted
  std::unordered_set<std::shared_ptr<CoreContext>> enumerated;

  // These are the elements eliminated from the vector.  By the time we're done the should all be expired.
  std::vector<std::weak_ptr<CoreContext>> eliminated;

  // Go through the enumeration, the totals should line up by the time we're done:
  for(const auto& cur : CurrentContextEnumerator()) {
    enumerated.insert(cur);

    // Pull off the last element:
    auto removed = children.back();
    children.pop_back();

    // If we haven't enumerated this element already, we want to mark it as eliminated-before-enumerated
    if(!enumerated.count(removed))
      eliminated.push_back(removed);
  }

  // Now verify that nothing we eliminated was enumerated:
  for(const auto& cur : eliminated)
    ASSERT_TRUE(cur.expired()) << "Found an element that was iterated after it should have been unreachable";
}

TEST_F(ContextEnumeratorTest, Unique) {
  AutoCreateContextT<NamedContext> named;
  ASSERT_EQ(named, ContextEnumeratorT<NamedContext>().unique()) <<
    "Expected the unique context to be equal to the specifically named child context";
}

TEST_F(ContextEnumeratorTest, BadUnique0) {
  // Intentionally create 0 contexts with the NamedContext sigil.
  ASSERT_THROW(ContextEnumeratorT<NamedContext>().unique(), autowiring_error) <<
    "An attempt to obtain a unique context from an enumerator providing none should throw an exception";
}

TEST_F(ContextEnumeratorTest, BadUnique2) {
  AutoCreateContextT<NamedContext> named1;
  AutoCreateContextT<NamedContext> named2;

  ASSERT_THROW(ContextEnumeratorT<NamedContext>().unique(), autowiring_error) <<
    "An attempt to obtain a unique context from an enumerator providing more than one should throw an exception";
}

TEST_F(ContextEnumeratorTest, ForwardIteratorCheck) {
  static_assert(std::is_default_constructible<ContextEnumerator::iterator>::value, "ForwardIterator constraint requires iterators to be default-constructable");
  static_assert(
    std::is_same<
      std::iterator_traits<ContextEnumerator::iterator>::iterator_category,
      std::forward_iterator_tag
    >::value,
    "ContextEnumerator iterator not recognized as a forward iterator"
  );

  AutoCreateContext ctxt1;
  AutoCreateContext ctxt2;
  AutoCreateContext ctxt2_0(ctxt2);
  AutoCreateContext ctxt2_1(ctxt2);
  AutoCreateContext ctxt2_1_0(ctxt2_1);
  AutoCreateContext ctxt3;

  ContextEnumerator e;
  auto a = e.begin();
  auto b = e.begin();

  ASSERT_EQ(a++, b++) << "Incrementation equivalence was not satisfied";
  ASSERT_EQ(a, b) << "Iterators not equivalent after incrementation";

  auto prior = *b;
  a++;
  ASSERT_EQ(prior, *b) << "Incrementation of an unrelated iterator invalidated a copy of that iterator";
  ++b;
  ASSERT_EQ(a, b) << "Postfix and prefix incrementation are not equivalently implemented";

  a++;
  ASSERT_EQ(a, std::next(b)) << "std::next did not correctly return the next iterator after the specified iterator";
  b++;

  std::advance(a, 1);
  b++;
  ASSERT_EQ(a, b) << "std::advance did not actually advance an iterator";
}
