// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
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

TEST_F(AutoFilterDescriptorTest, ArgumentTypeInferenceCheck) {
  AutoFilterDescriptor desc([](AutoPacket&, Decoration<0>&, int, const Decoration<1>&, std::shared_ptr<const float>) {});
  ASSERT_EQ(5UL, desc.GetArity()) << "Descriptor did not extract the correct argument count";

  // Verify the general shape of things:
  size_t nIn = 0;
  size_t nOut = 0;
  size_t nTotal = 0;
  const auto* pFirstArg = desc.GetAutoFilterArguments();
  for (auto* pCur = pFirstArg; *pCur; pCur++) {
    nTotal++;
    if (pCur->is_input)
      nIn++;
    if (pCur->is_output)
      nOut++;
  }

  // Note:  AutoPacket is not considered an input argument, this is why inputs + outputs != arity
  ASSERT_EQ(5UL, nTotal) << "AutoFilter argument array didn't appear to contain the correct number of arguments";
  ASSERT_EQ(3UL, nIn) << "Input argument count mismatch";
  ASSERT_EQ(1UL, nOut) << "Output argument count mismatch";

  // Now verify specific args:
  ASSERT_EQ(pFirstArg[0].id, auto_id_t<AutoPacket>{});
  ASSERT_EQ(pFirstArg[1].id, auto_id_t<Decoration<0>>{});
  ASSERT_EQ(pFirstArg[2].id, auto_id_t<int>{});
  ASSERT_EQ(pFirstArg[3].id, auto_id_t<Decoration<1>>{});
  ASSERT_EQ(pFirstArg[4].id, auto_id_t<float>{});
}
