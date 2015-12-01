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

TEST_F(AutoFilterRvalueTest, CanUseInTheChain) {
  AutoRequired<AutoPacketFactory> factory;

  *factory += [](Decoration<0> dec0, Decoration<1>& dec1) {
    dec1.i = dec0.i;
  };

  // This is the filter that does the modification
  *factory += [](Decoration<1>&& dec1) {
    dec1.i = 999;
  };

  *factory += [](Decoration<1> dec1, Decoration<2>& dec2) {
    dec2.i = dec1.i;
  };

  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>{100});
  const Decoration<2>& dec2 = packet->Get<Decoration<2>>();
  ASSERT_EQ(999, dec2.i) << "R-value AutoFilter was not able to modify a decoration value before passing on to the next Autofilter";
}

TEST_F(AutoFilterRvalueTest, MultipleModifiersWithSameAltitude) {
  AutoRequired<AutoPacketFactory> factory;

  *factory += [](Decoration<0>&& dec0) {
    dec0.i = 999;
  };

  *factory += [](Decoration<1> dec1, Decoration<0>&& dec0) {
    dec0.i = dec1.i;
  };

  *factory += [](Decoration<0> dec0) {};

  ASSERT_THROW(factory->NewPacket(), autowiring_error) << "An exception should have been thrown when there are multiple r-value AutoFilter with the same altitude for an existing subscriber";
}

TEST_F(AutoFilterRvalueTest, MultipleModifiersWithDifferentAltitude) {
  AutoRequired<AutoPacketFactory> factory;
  int called = 0;

  *factory += [&](Decoration<1> dec1, Decoration<0>& dec0) {
    called++;
    dec0.i = dec1.i;
  };

  *factory += [&](Decoration<0>&& dec0) {
    called++;
    dec0.i = 999;
  };

  *factory += autowiring::altitude::Lowest, [&](Decoration<0>&& dec0) {
    called++;
    dec0.i = 1000;
  };

  *factory += [&](Decoration<0> dec0) {
    called++;
  };

  auto packet = factory->NewPacket();
  packet->AddRecipient(AutoFilterDescriptor([&] (Decoration<0>&& dec0) {
    called++;
    dec0.i = 2000;
  }, autowiring::altitude::Highest));

  packet->Decorate(Decoration<1>{555});
  ASSERT_EQ(5, called) << "AutoFilters was not called as expected when there are multiple R-value AutoFilter with different altitude ";

  const Decoration<0>& dec0 = packet->Get<Decoration<0>>();
  ASSERT_EQ(1000, dec0.i) << "AutoFilters was not called in the correct order when there are multiple R-value AutoFilter with different altitude";
}

TEST_F(AutoFilterRvalueTest, DetectCycle) {
  AutoRequired<AutoPacketFactory> factory;

  *factory += [](Decoration<0> dec0, Decoration<1>&& dec1) {
    dec1.i = dec0.i;
  };

  *factory += [](Decoration<1> dec1, Decoration<0>&& dec0) {
    dec0.i = dec1.i;
  };

  ASSERT_THROW(factory->NewPacket(), autowiring_error) << "An exception should have been thrown when there is a cycle in the AutoFilter graph";
}

TEST_F(AutoFilterRvalueTest, RecipientRemovalTest) {
  AutoRequired<AutoPacketFactory> factory;
  auto called = std::make_shared<bool>(false);

  // Add a recipient and then remove it, verify it doesn't get called
  auto packet = factory->NewPacket();
  const auto recipient =
  (
   *packet += [called] (Decoration<0>&&) {
     *called = true;
   }
  );
  packet->RemoveRecipient(*recipient);
  packet->Decorate(Decoration<0>());

  ASSERT_FALSE(*called) << "A recipient that should have been removed was called";
}
