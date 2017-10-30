// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/Decoration.hpp"
#include <autowiring/autowiring.h>

class AutoFilterAltitudeTest:
  public testing::Test
{};

struct AltitudeValue {};

struct AltitudeMonotonicCounter {
  std::atomic<int> order{0};
};

template<autowiring::altitude A>
struct HasProfilingAltitude {
  static const autowiring::altitude altitude = A;

  AutoRequired<AltitudeMonotonicCounter> ctr;
  int order = -1;

  void AutoFilter(const AltitudeValue& val) {
    order = ++ctr->order;
  }
};

TEST_F(AutoFilterAltitudeTest, AltitudeDetection) {
  AutoFilterDescriptor desc(std::make_shared<HasProfilingAltitude<autowiring::altitude::Highest>>());
  ASSERT_EQ(autowiring::altitude::Highest, desc.GetAltitude()) << "Filter altitude was not correctly inferred";
}

TEST_F(AutoFilterAltitudeTest, StandardAltitudeArrangement) {
  AutoCurrentContext()->Initiate();

  AutoRequired<HasProfilingAltitude<autowiring::altitude::Standard>> alt3;
  AutoRequired<HasProfilingAltitude<autowiring::altitude::Asynchronous>> alt1;
  AutoRequired<HasProfilingAltitude<autowiring::altitude::Realtime>> alt2;
  AutoRequired<HasProfilingAltitude<autowiring::altitude::Passive>> alt4;
  AutoRequired<HasProfilingAltitude<autowiring::altitude::Lowest>> alt5;
  AutoRequired<HasProfilingAltitude<autowiring::altitude::Dispatch>> alt0;

  AutoRequired<AutoPacketFactory> factory;
  auto packet = factory->NewPacket();
  packet->Decorate(AltitudeValue{});

  // Now we verify things got invoked in the right order:
  ASSERT_EQ(1, alt0->order);
  ASSERT_EQ(2, alt1->order);
  ASSERT_EQ(3, alt2->order);
  ASSERT_EQ(4, alt3->order);
  ASSERT_EQ(5, alt4->order);
  ASSERT_EQ(6, alt5->order);
}

TEST_F(AutoFilterAltitudeTest, LambdaAltitudesOnFactory) {
  AutoCurrentContext()->Initiate();
  AutoRequired<AutoPacketFactory> factory;

  int seq = 0;
  int ctr[9];

  for (size_t i = 0; i < 9; i++) {
    ctr[i] = -1;
    *factory += (autowiring::altitude)i, [&, i] {
      ctr[i] = ++seq;
    };
  }

  // Generate a packet and trip the assignments:
  auto packet = factory->NewPacket();
  ASSERT_EQ(1, ctr[0]);
  ASSERT_EQ(2, ctr[1]);
  ASSERT_EQ(3, ctr[2]);
  ASSERT_EQ(4, ctr[3]);
  ASSERT_EQ(5, ctr[4]);
  ASSERT_EQ(6, ctr[5]);
  ASSERT_EQ(7, ctr[6]);
  ASSERT_EQ(8, ctr[7]);
  ASSERT_EQ(9, ctr[8]);
}

TEST_F(AutoFilterAltitudeTest, AltitudeOnPacket) {
  AutoCurrentContext()->Initiate();
  AutoRequired<AutoPacketFactory> factory;

  int seq = 0;
  int ctr[4] = {-1, -1, -1, -1};

  *factory += autowiring::altitude::Standard, [&] (const Decoration<0>& dec) {
    ctr[0] = ++seq;
  };
  *factory += autowiring::altitude::Lowest, [&] (const Decoration<0>& dec) {
    ctr[1] = ++seq;
  };

  auto packet = factory->NewPacket();

  packet->AddRecipient(AutoFilterDescriptor([&] (const Decoration<0>& dec) {
    ctr[2] = ++seq;
  }, autowiring::altitude::Dispatch));
  packet->AddRecipient(AutoFilterDescriptor([&] (const Decoration<0>& dec) {
    ctr[3] = ++seq;
  }, autowiring::altitude::Passive));

  packet->Decorate(Decoration<0>());

  ASSERT_EQ(2, ctr[0]);
  ASSERT_EQ(4, ctr[1]);
  ASSERT_EQ(1, ctr[2]);
  ASSERT_EQ(3, ctr[3]);
}
