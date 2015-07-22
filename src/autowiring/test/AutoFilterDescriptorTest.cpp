// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/Decoration.hpp"
#include <autowiring/AutoFilterDescriptor.h>

class AutoFilterDescriptorTest:
  public testing::Test
{};

TEST_F(AutoFilterDescriptorTest, DescriptorNonEquivalence) {
  AutoFilterDescriptor descs[2];
  for (size_t i = 0; i < 2; i++)
    descs[i] = AutoFilterDescriptor([i](int) {});
  ASSERT_NE(descs[0], descs[1]) << "Descriptors evaluated to equivalence even though they were constructed distinctly";
  ASSERT_NE(descs[0].GetAutoFilter(), descs[1].GetAutoFilter()) << "Shared pointers to underlying autofilters were equal when they should not have been";
  ASSERT_NE(descs[0] < descs[1], descs[1] < descs[0]) << "Two inequal descriptors violated disjunctive syllogism";
}

TEST_F(AutoFilterDescriptorTest, ArityCheck) {
  AutoFilterDescriptor desc([](int, Decoration<0>&, const Decoration<1>&) {});
  ASSERT_EQ(3UL, desc.GetArity()) << "Descriptor did not extract the correct argument count";

  size_t nIn = 0;
  size_t nOut = 0;
  size_t nTotal = 0;
  for (auto* pCur = desc.GetAutoFilterArguments(); *pCur; pCur++) {
    nTotal++;
    if (pCur->is_input)
      nIn++;
    if (pCur->is_output)
      nOut++;
  }

  ASSERT_EQ(3UL, nTotal) << "AutoFilter argument array didn't appear to contain the correct number of arguments";
  ASSERT_EQ(2UL, nIn) << "Input argument count mismatch";
  ASSERT_EQ(1UL, nOut) << "Output argument count mismatch";
}
