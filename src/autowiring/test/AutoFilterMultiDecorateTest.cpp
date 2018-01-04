// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/CoreThread.h>
#include "TestFixtures/Decoration.hpp"
#include CHRONO_HEADER
#include THREAD_HEADER

class AutoFilterMultiDecorateTest:
  public testing::Test
{
public:
  AutoFilterMultiDecorateTest(void) {
    AutoCurrentContext()->Initiate();
  }

  AutoRequired<AutoPacketFactory> factory;
};

TEST_F(AutoFilterMultiDecorateTest, EnumerateDecorationsTest) {
  auto sample = [](const int* vals []) {};
  AutoFilterDescriptor desc(sample);

  size_t i = 0;
  for (auto* pCur = desc.GetAutoFilterArguments(); *pCur; pCur++)
    i++;

  ASSERT_EQ(1, i) << "AutoFilterDescriptor parsed an incorrect number of arguments from a lambda";
}

TEST_F(AutoFilterMultiDecorateTest, MultiDecorateTest) {
  int called = 0;

  *factory += [&called](int& out) { out = called++; };
  *factory += [&called](int& out) { out = called++; };
  *factory += [&called](const int* vals []) {
    ASSERT_NE(nullptr, vals);
    called++;

    // Guarantee that values were added in the expected order
    int i;
    for (i = 0; vals[i]; i++)
      ASSERT_EQ(i, *(vals[i])) << "Incorrect values were added to the packet";

    // Verify we got the number of values out that we wanted to get out
    ASSERT_EQ(2, i) << "The wrong number of values were added to the packet";
  };
  ASSERT_EQ(0, called) << "Lambda functions were called before expected";

  auto packet = factory->NewPacket();
  ASSERT_EQ(3, called) << "Not all lambda functions were called as expected";
}

TEST_F(AutoFilterMultiDecorateTest, MultiWithSingleDecorateTest) {
  int called = 0;
  int f_called = 0;

  *factory += [&called](AutoPacket& packet, Decoration<0>& out) {
    ASSERT_FALSE(packet.IsUnsatisfiable<Decoration<0>>());
    ASSERT_FALSE(packet.IsUnsatisfiable<Decoration<1>>());
    out.i = called++;
  };

  *factory += [&called](AutoPacket& packet, Decoration<0>& out) {
    ASSERT_FALSE(packet.IsUnsatisfiable<Decoration<0>>());
    ASSERT_FALSE(packet.IsUnsatisfiable<Decoration<1>>());
    out.i = called++;
  };

  *factory += [&f_called](AutoPacket& packet, Decoration<1>& out) {
    ASSERT_FALSE(packet.IsUnsatisfiable<Decoration<0>>());
    ASSERT_FALSE(packet.IsUnsatisfiable<Decoration<1>>());
    out.i = ++f_called;
  };

  *factory += [&called](const Decoration<1>& val, const Decoration<0>* vals []) {
    ASSERT_NE(nullptr, vals);
    called++;

    // Guarantee that values were added in the expected order
    int i;
    for (i = 0; vals[i]; i++)
      ASSERT_EQ(i, vals[i]->i) << "Incorrect values were added to the packet";

    // Verify we got the number of values out that we wanted to get out
    ASSERT_EQ(2, i) << "The wrong number of values were added to the packet";
    ASSERT_EQ(1, val.i) << "The wrong number of val were added to the packet";
  };
  ASSERT_EQ(0, called) << "Lambda functions were called before expected";

  // Verify internal integrity of the factory using reflection:
  int nOutputDec0 = 0;
  int nOutputDec1 = 0;
  for (auto& filter : factory->GetAutoFilters())
    for (auto arg = filter.GetAutoFilterArguments(); *arg; arg++)
      if(arg->is_output) {
        if (*arg->id.block->ti == typeid(Decoration<0>))
          nOutputDec0++;
        else if (*arg->id.block->ti == typeid(Decoration<1>))
          nOutputDec1++;
      }

  ASSERT_EQ(2UL, nOutputDec0) << "Counted an incorrect number of Decoration<0> publishers";
  ASSERT_EQ(1UL, nOutputDec1) << "Counted an incorrect number of Decoration<1> publishers";

  auto packet = factory->NewPacket();
  ASSERT_EQ(3, called) << "Not all lambda functions were called as expected";
}

TEST_F(AutoFilterMultiDecorateTest, MultiPostHocIntroductionTest) {
  int called = 0;

  *factory += [&called](int& out) { out = called++; };
  *factory += [&called](int& out) { out = called++; };

  // Add a gather step on the packet:
  auto packet = factory->NewPacket();
  *packet += [&called](const int* vals []){
    ASSERT_NE(nullptr, vals);
    called++;
  };

  ASSERT_EQ(3, called) << "Not all lambda functions were called as expected";
}

TEST_F(AutoFilterMultiDecorateTest, UnsatDecTest) {
  AutoRequired<AutoPacketFactory> f;
  *f += [] (const Decoration<0>&, std::string& out) {
    out = "Hello";
  };
  *f += [](const Decoration<1>&, std::string& out) {
    out = "World";
  };
  *f += [](const Decoration<2>&, std::string& out) {
    out = "Crickets";
  };

  int f1 = 0;
  *f += [&](const std::string* args []) {
    for (f1 = 0; *args; args++, f1++);
  };

  int f2 = 0;
  *f += [&](const std::string*const args []) {
    for (f2 = 0; *args; args++, f2++);
  };

  auto packet = f->NewPacket();
  packet->Decorate(Decoration<0>{});
  packet->MarkUnsatisfiable<Decoration<1>>();
  packet->Decorate(Decoration<2>{});

  auto strs = packet->GetAll<std::string>();
  ASSERT_NE(nullptr, strs) << "String datatype not found on multidecorate packet";
  ASSERT_NE(nullptr, strs[0]) << "No strings attached to a multidecorate packet as expected";
  ASSERT_NE(nullptr, strs[1]) << "Expected two strings back, got one";
  ASSERT_EQ(nullptr, strs[2]) << "Expected two strings back, got three";
  ASSERT_EQ("Hello", *strs[0]) << "Entry in multidecorate set was not the expected value";
  ASSERT_EQ("Crickets", *strs[1]) << "Entry in multidecorate set was not the expected value";

  ASSERT_EQ(2, f1) << "const T*[] input decoration count mismatch";
  ASSERT_EQ(2, f2) << "const T*const[] input decoration count mismatch";
}

TEST_F(AutoFilterMultiDecorateTest, ArrayOfSharedPointers) {
  AutoRequired<AutoPacketFactory> f;
  *f += [](const Decoration<0>&, std::string& out) {
    out = "Hello";
  };
  *f += [](const Decoration<1>&, std::string& out) {
    out = "World";
  };

  int f1 = 0;
  *f += [&](std::shared_ptr<const std::string> args []) {
    for (f1 = 0; *args; args++, f1++);
  };

  int f2 = 0;
  *f += [&](const std::shared_ptr<const std::string> args []) {
    for (f2 = 0; *args; args++, f2++);
  };

  auto packet = f->NewPacket();
  packet->Decorate(Decoration<0>{});
  packet->Decorate(Decoration<1>{});

  ASSERT_EQ(2, f1) << "shared_ptr[] input decoration count mismatch";
  ASSERT_EQ(2, f2) << "const shared_ptr[] input decoration count mismatch";
}

class SameOutputTwiceWithInput {
public:
  void AutoFilter(Decoration<0>& one, Decoration<0>& two) {
    one.i = 101;
    two.i = 102;
  }
};

static_assert(has_unambiguous_autofilter<SameOutputTwiceWithInput>::value, "AutoFilter appears to be ambiguous on a multi-out type");
static_assert(has_autofilter<SameOutputTwiceWithInput>::value, "AutoFilter not detected on a multi-out class");

TEST_F(AutoFilterMultiDecorateTest, SingleFunctionDoubleOutput) {
  AutoRequired<AutoPacketFactory> factory;
  AutoCurrentContext()->Initiate();

  AutoRequired<SameOutputTwiceWithInput>();
  *factory += [](Decoration<0>& one, Decoration<0>& two) {
    ASSERT_NE(&one, &two) << "When the same decoration type is mentioned more than once in a signature, the two outputs should be distinct";
    one.i = 1;
    two.i = 2;
  };

  size_t nArgs = 0;
  *factory += [&] (const Decoration<0>* args []) {
    while (*args++)
      nArgs++;
  };

  factory->NewPacket();
  ASSERT_EQ(4UL, nArgs) << "Two filters that multiply attach decorations did not correctly do so";
}

TEST_F(AutoFilterMultiDecorateTest, AddRecipientIdempotentTest) {
  AutoRequired<AutoPacketFactory> factory;
  AutoCurrentContext()->Initiate();

  int called = 0;
  auto desc = AutoFilterDescriptor(
    [&called](Decoration<0> one) {
      called++;
    }
  );

  factory->AddSubscriber(desc);
  factory->AddSubscriber(desc);

  auto packet = factory->NewPacket();

  packet->AddRecipient(desc);
  packet->AddRecipient(desc);

  packet->Decorate(Decoration<0>());

  ASSERT_EQ(1, called) << "Add receipient should be idempotent";
}

struct MultiDecoration {};

TEST_F(AutoFilterMultiDecorateTest, ManyMultiDecorate) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  AutoRequired<AutoPacketFactory> factory;

  // This lambda is invoked every time a packet is created.  It doesn't attach Decoration<0>
  // to the packet, and as a result, Decoration<0> will be marked unsatisfiable.
  *factory += [](std::shared_ptr<Decoration<0>>&) {};

  // These won't ever be called.  Decoration<0> is unsatisfiable, so Decoration<1> is
  // transitively unsatisfiable, and therefore so is Decoration<2>.  The critical requirement
  // is that MultiDecoration not be marked unsatisfiable at this point.
  *factory += [](const Decoration<0>&, Decoration<1>& sma) {};
  *factory += [](const Decoration<1>&, Decoration<2>& pMap) {};
  *factory += [](const Decoration<0>&, const Decoration<1>&, MultiDecoration&) {};

  // This one gets called as soon as Decoration<2> is marked unsatisfiable.  It should still
  // be able to attach MultiDecoration to the packet
  *factory += [](const std::shared_ptr<const Decoration<2>>&, MultiDecoration&) {};

  // Create a few packets, ensure none of these creations fail
  // Don't reduce these into the following loop, having these here allows a convenient
  // location for breakpoints to be set.
  factory->NewPacket();
  factory->NewPacket();

  // Create a bunch more in a loop
  for(size_t i = 0; i < 100; i++)
    factory->NewPacket();
}
