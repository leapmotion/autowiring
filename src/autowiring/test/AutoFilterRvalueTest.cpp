// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
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

  AutoRequired<AutoPacketFactory> factory;
};

TEST_F(AutoFilterRvalueTest, SimpleCallCheck) {
  // Register an r-value filter that will receive our decoration
  bool called = false;
  *factory += [&] (Decoration<0>&& dec) { called = true; };

  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>{});
  ASSERT_TRUE(called) << "R-value AutoFilter was not called as expected";
}

TEST_F(AutoFilterRvalueTest, CanModifyInPlace) {
  // Register an r-value filter that will receive our decoration
  *factory += [&](Decoration<0>&& dec) { dec.i = 129; };

  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>{292});
  const Decoration<0>& dec = packet->Get<Decoration<0>>();
  ASSERT_EQ(129, dec.i) << "AutoFilter was not able to modify a decoration value in-place";
}

TEST_F(AutoFilterRvalueTest, CallOrderCorrect) {
  std::vector<std::pair<size_t, Decoration<0>>> observations;

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
  int called = 0;
  int order[] = {-1, -1, -1, -1, -1, -1};

  *factory += [&](Decoration<1> dec1, Decoration<0>& dec0) {
    dec0.i = dec1.i;
    order[0] = called++;
  };

  *factory += [&](Decoration<0>&& dec0) {
    dec0.i = 999;
    order[1] = called++;
  };

  *factory += autowiring::altitude::Lowest, [&](Decoration<0>&& dec0) {
    dec0.i = 1000;
    order[2] = called++;
  };

  *factory += [&](Decoration<0> dec0) {
    order[3] = called++;
  };

  auto packet = factory->NewPacket();
  packet->AddRecipient(AutoFilterDescriptor([&] (Decoration<0>&& dec0) {
    dec0.i = 2000;
    order[4] = called++;
  }, autowiring::altitude::Highest));

  packet->AddRecipient(AutoFilterDescriptor([&] (Decoration<0>&& dec0) {
    dec0.i = 2000;
    order[5] = called++;
  }, autowiring::altitude::Realtime));

  packet->Decorate(Decoration<1>{555});
  ASSERT_EQ(6, called) << "AutoFilters was not called as expected when there are multiple R-value AutoFilter with different altitude ";

  int expected[] = {0, 3, 4, 5, 1, 2};
  for (int i = 0; i < 6; i++)
    EXPECT_EQ(expected[i], order[i]);

  const Decoration<0>& dec0 = packet->Get<Decoration<0>>();
  ASSERT_EQ(1000, dec0.i) << "AutoFilters was not called in the correct order when there are multiple R-value AutoFilter with different altitude";
}

TEST_F(AutoFilterRvalueTest, DetectCycle) {
  *factory += [](Decoration<0> dec0, Decoration<1>&& dec1) {
    dec1.i = dec0.i;
  };

  *factory += [](Decoration<1> dec1, Decoration<0>&& dec0) {
    dec0.i = dec1.i;
  };

  ASSERT_THROW(factory->NewPacket(), autowiring_error) << "An exception should have been thrown when there is a cycle in the AutoFilter graph";
}

TEST_F(AutoFilterRvalueTest, RecipientRemovalTest) {
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

TEST_F(AutoFilterRvalueTest, SharedPtrTest) {
  auto called0 = std::make_shared<bool>(false);
  *factory += [called0] (Decoration<0> in, std::shared_ptr<Decoration<1>>& out) {
    out.reset(new Decoration<1>);
    out->i = in.i;
    *called0 = true;
  };

  auto called1 = std::make_shared<bool>(false);
  *factory += [called1] (std::shared_ptr<Decoration<1>>&& sp) {
    sp.reset(new Decoration<1>);
    *called1 = true;
  };

  auto called2 = std::make_shared<bool>(false);
  *factory += [called2] (Decoration<1> in) {
    *called2 = true;
  };

  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>());
  ASSERT_TRUE(*called0);
  ASSERT_TRUE(*called1);
  ASSERT_TRUE(*called2);
  const Decoration<1>& dec1 = packet->Get<Decoration<1>>();
  ASSERT_EQ(1, dec1.i) << "R-value shared pointer AutoFilter was not able to modify a decoration value before passing on to the next Autofilter";
}

TEST_F(AutoFilterRvalueTest, SharedPtrAltitudeTest) {
  int called = 0;
  int results[] = {-1, -1, -1, -1, -1, -1};

  // This should get called first
  *factory += [&] (Decoration<0> in, std::shared_ptr<Decoration<1>>& out) {
    out.reset(new Decoration<1>);
    out->i = 0;
    results[called++] = out->i;
  };

  // This should get called third
  *factory += autowiring::altitude::Dispatch, [&](std::shared_ptr<Decoration<1>>&& sp) {
    sp.reset(new Decoration<1>);
    sp->i = 1;
    results[called++] = sp->i;
  };

  // This should get called forth
  *factory += autowiring::altitude::Realtime, [&](Decoration<1>&& dec1) {
    dec1.i = 2;
    results[called++] = dec1.i;
  };

  // This should get called second
  *factory += autowiring::altitude::Highest, [&](Decoration<1>&& dec1) {
    dec1.i = 3;
    results[called++] = dec1.i;
  };

  // This should get called fifth
  *factory += [&] (std::shared_ptr<Decoration<1>>&& sp) {
    sp.reset();
    called++;
  };

  // This should never gets called because of the reset above
  *factory += autowiring::altitude::Lowest, [&](Decoration<1> in) {
    results[called++] = in.i;
  };

  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>());
  ASSERT_EQ(5, called);

  int expected[] = {0, 3, 1, 2, -1, -1};
  for (int i = 0; i < 6; i++)
    ASSERT_EQ(expected[i], results[i]);
}
