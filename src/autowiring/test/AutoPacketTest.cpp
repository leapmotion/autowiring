// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/CoreThread.h>
#include "TestFixtures/Decoration.hpp"

class AutoPacketTest:
  public testing::Test
{
public:
  AutoPacketTest(void) {
    AutoCurrentContext()->Initiate();
  }
  AutoRequired<AutoPacketFactory> factory;
};

TEST_F(AutoPacketTest, SimpleCallTest) {
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>());
  std::condition_variable cv;

  // Factory call:
  ASSERT_TRUE(packet->Wait<Decoration<0>>(std::chrono::milliseconds(0), cv)) << "Unexpected timeout";
}

TEST_F(AutoPacketTest, FactoryCallTest) {
  AutoRequired<FilterGen<const Decoration<0>&, Decoration<1>&>> fg;
  auto packet = factory->NewPacket();

  // Blocking factory call:
  std::condition_variable cv;
  bool bCalled = false;
  packet->Decorate(Decoration<0>());
  
  ASSERT_TRUE(
    packet->Wait(
      cv,
      [&bCalled](const Decoration<1>&) {
        bCalled = true;
      },
      std::chrono::nanoseconds(0)
    )
  );

  ASSERT_TRUE(bCalled);
}

TEST_F(AutoPacketTest, MultipleDecorateGetFailures) {
  // Decorate with two integer types:
  *factory += [](int& arg) { arg = 0; };
  *factory += [](int& arg) { arg = 1; };

  // Now issue the packet:
  auto packet = factory->NewPacket();

  // Any type of "Get" call made on int should fail now
  {
    const int* out;
    ASSERT_ANY_THROW(packet->Get(out));
  }
  {
    const std::shared_ptr<const int>* out;
    ASSERT_ANY_THROW(packet->Get(out));
  }
}

TEST_F(AutoPacketTest, AliasGet) {
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>{});

  const std::shared_ptr<const Decoration<0>>* ptr;
  ASSERT_TRUE(packet->Get(ptr)) << "Failed to find the shared pointer version of a trivial decoration on the packet";
}

TEST_F(AutoPacketTest, CurrentPacket) {
  AutoRequired<AutoPacketFactory> factory;
  *factory += [](AutoPacket& packet) {
    AutoPacket& current = *(Autowired<AutoPacketFactory>()->CurrentPacket());
    ASSERT_EQ(&packet, &current) << "Current packet request did not correctly return a pointer to the current packet";
  };

  auto packet = factory->NewPacket();
  ASSERT_TRUE(packet.unique()) << "A reference to a packet was leaked after all processing should have been done";
  ASSERT_THROW(AutoPacket::CurrentPacket(), autowiring_error) << "An attempt to retrieve the current packet outside of an AutoFilter unexpectedly succeeded";
}
