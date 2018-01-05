// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
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
    AutoPacket& current = AutoPacket::CurrentPacket();
    ASSERT_EQ(&packet, &current) << "Current packet request did not correctly return a pointer to the current packet";
  };

  auto packet = factory->NewPacket();
  ASSERT_TRUE(packet.unique()) << "A reference to a packet was leaked after all processing should have been done";
  ASSERT_THROW(AutoPacket::CurrentPacket(), autowiring_error) << "An attempt to retrieve the current packet outside of an AutoFilter unexpectedly succeeded";
}

TEST_F(AutoPacketTest, NoLeakUnderThrow) {
  AutoRequired<AutoPacketFactory> factory;

  std::shared_ptr<AutoPacket> ptr;
  *factory += [&](AutoPacket& packet) {
    ptr = packet.shared_from_this();
    throw std::runtime_error("");
  };

  ASSERT_THROW(factory->NewPacket(), std::runtime_error) << "A filter that throws exeptions was not caught correctly";
  ASSERT_TRUE(ptr.unique()) << "A packet was leaked when generated by a filter that throws an exception";
  ASSERT_THROW(AutoPacket::CurrentPacket(), autowiring_error) << "A dumb pointer was incorrectly leaked";
}


TEST_F(AutoPacketTest, CurrentPacketUnderDecoration) {
  AutoRequired<AutoPacketFactory> factory;
  *factory += [](const int arg, AutoPacket& packet) {
    AutoPacket& current = AutoPacket::CurrentPacket();
    ASSERT_EQ(&packet, &current) << "Current packet request did not correctly return a pointer to the current packet";
  };

  auto packet = factory->NewPacket();
  packet->Decorate(10);
  ASSERT_TRUE(packet.unique()) << "A reference to a packet was leaked after all processing should have been done";
  ASSERT_THROW(AutoPacket::CurrentPacket(), autowiring_error) << "An attempt to retrieve the current packet outside of an AutoFilter unexpectedly succeeded";
}

TEST_F(AutoPacketTest, ComplexNetwork) {
  AutoRequired<AutoPacketFactory> factory;
  *factory += [](Decoration<0>&) {};


  AutoPacket* pPacketA = nullptr;
  *factory += [&pPacketA](const Decoration<0>&, Decoration<1>&) {
    pPacketA = &AutoPacket::CurrentPacket();
  };

  AutoPacket* pPacketB = nullptr;
  *factory += [&pPacketB](const Decoration<0>&, Decoration<2>&) {
    pPacketB = &AutoPacket::CurrentPacket();
  };

  AutoPacket* pPacketAA = nullptr;
  *factory += [&pPacketAA](const Decoration<1>&) {
    pPacketAA = &AutoPacket::CurrentPacket();
  };

  std::shared_ptr<AutoPacket> packet;
  ASSERT_NO_THROW(packet = factory->NewPacket()) << "Packet construction threw an exception unexpectedly";

  ASSERT_EQ(pPacketA, packet.get()) << "Current packet in first second-level filter call was not equal as expected";
  ASSERT_EQ(pPacketB, packet.get()) << "Current packet in second second-level filter call was not equal as expected";
  ASSERT_EQ(pPacketAA, packet.get()) << "Current packet in third-level filter call was not equal as expected";
}

TEST_F(AutoPacketTest, CallTest) {
  AutoRequired<AutoPacketFactory> factory;
  auto packet = factory->NewPacket();

  packet->Decorate(Decoration<0>{101});
  packet->Decorate(Decoration<1>{102});

  bool called = false;
  Decoration<0> rd0;
  Decoration<1> rd1;
  
  Decoration<2> rd2;

  packet->Call(
    [&](const Decoration<0>& d0, Decoration<1> d1, Decoration<2>& d2) {
      called = true;
      rd0 = d0;
      rd1 = d1;
      d2 = Decoration<2>{ 299 };
    },
    rd2
  );

  ASSERT_TRUE(called) << "Call-by lambda was not invoked as expected";
  ASSERT_EQ(101, rd0.i) << "Decoration<0> was not properly copied into a call";
  ASSERT_EQ(102, rd1.i) << "Decoration<1> was not properly copied into a call";
  ASSERT_EQ(299, rd2.i) << "Decoration<2> was not extracted from the call filter properly";
}

static void SimpleCall(std::shared_ptr<const Decoration<4>> d4, const Decoration<0>& d0, Decoration<1>& d1, Decoration<2>& d2, Decoration<3>& unused) {
  d1.i = d4->i;
  d2.i = d0.i;
  unused.i = 9999;
}

static_assert(auto_arg<Decoration<2>&>::is_output, "Output type not correctly detected");

static_assert(
  autowiring::choice<Decoration<2>&, std::tuple<const Decoration<0>&, Decoration<1>&, Decoration<2>&>>::is_matched,
  "Failed to match an obvious choice output"
);

TEST_F(AutoPacketTest, ObjectCallTest) {
  AutoRequired<AutoPacketFactory> factory;
  auto packet = factory->NewPacket();
  packet->Decorate(Decoration<0>{1001});
  packet->Decorate(Decoration<4>{9001});

  Decoration<1> d1;
  Decoration<2> d2;
  packet->Call(SimpleCall, d1, d2);

  ASSERT_EQ(9001, d1.i) << "Moore value not assigned correctly";
  ASSERT_EQ(1001, d2.i) << "Mealy value not assigned correctly";
}

namespace {
  class CountsCopies {
  public:
    CountsCopies(void) {
      s_nConstructions++;
    }

    CountsCopies(CountsCopies& rhs) :
      nCopies(rhs.nCopies + 1),
      value(rhs.value)
    {}

    void operator=(const CountsCopies& rhs) {
      nCopies = rhs.nCopies + 1;
      value = rhs.value;
    }

    static size_t s_nConstructions;

    int nCopies = 0;
    int value = 2;
  };

  size_t CountsCopies::s_nConstructions = 0;
}

TEST_F(AutoPacketTest, NoUnneededOutputCopy) {
  CountsCopies::s_nConstructions = 0;

  AutoRequired<AutoPacketFactory> factory;
  auto packet = factory->NewPacket();

  packet->Decorate(Decoration<0>{101});

  CountsCopies rcc;
  CountsCopies* pRcc = nullptr;

  packet->Call(
    [&](const Decoration<0>& d0, CountsCopies& cc) {
      cc.value = 109;
      pRcc = &cc;
    },
    rcc
  );

  ASSERT_EQ(1UL, CountsCopies::s_nConstructions) << "An unexpected number of default constructed entities was created";
  ASSERT_EQ(&rcc, pRcc) << "Destination value was not found at the correct address";
  ASSERT_EQ(109, rcc.value) << "Copy-counting output value was not copied correctly";
  ASSERT_EQ(0UL, rcc.nCopies) << "An unnecessary number of copies was made during an extracting call";
}
