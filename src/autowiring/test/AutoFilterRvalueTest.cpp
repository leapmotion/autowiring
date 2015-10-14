// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/autowiring.h>
#include <autowiring/SatCounter.h>
#include "TestFixtures/Decoration.hpp"

class AutoFilterRvalueTest :
  public testing::Test
{
public:
  AutoFilterRvalueTest(void) {
    AutoCurrentContext()->Initiate();
  }
};

TEST_F(AutoFilterRvalueTest, SimpleCallCheck) {
  AutoRequired<AutoPacketFactory> factory;

  // Register an r-value filter that will receive our decoration
  bool called = false;
  *factory += [&] (Decoration<0>&& dec) { called = true; };

  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>{});
  ASSERT_TRUE(called) << "R-value AutoFilter was not called as expected";
}

TEST_F(AutoFilterRvalueTest, CanModifyInPlace) {
  AutoRequired<AutoPacketFactory> factory;

  // Register an r-value filter that will receive our decoration
  *factory += [&](Decoration<0>&& dec) { dec.i = 129; };

  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>{292});
  const Decoration<0>& dec = packet->Get<Decoration<0>>();
  ASSERT_EQ(129, dec.i) << "AutoFilter was not able to modify a decoration value in-place";
}

TEST_F(AutoFilterRvalueTest, CallOrderCorrect) {
  std::vector<std::pair<size_t, Decoration<0>>> observations;
  AutoRequired<AutoPacketFactory> factory;

  // Register a bunch of lambdas that will take observations of the decoration
  for (size_t i = 0; i < 10; i++)
    *factory += [&, i](Decoration<0> dec) { observations.push_back({ i, dec }); };

  // This is the filter that does the modification:
  *factory += [](Decoration<0>&& dec) { dec.i = 9999; };

  // Kick off, verify that filters all ran in the right order:
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>{100});
  for (const auto& observation : observations)
    ASSERT_EQ(9999, observation.second.i) << "AutoFilter " << observation.first << " ran before an r-value AutoFilter on the same altitude";
}
