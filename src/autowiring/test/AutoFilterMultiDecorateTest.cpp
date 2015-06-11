// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
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
  *f += [](const std::string* args[], int& val) {
    for (val = 0; *args; args++, val++);
  };

  auto packet = f->NewPacket();
  packet->Decorate(Decoration<0>{});
  packet->Unsatisfiable<Decoration<1>>();
  packet->Decorate(Decoration<2>{});

  auto strs = packet->GetAll<std::string>();
  ASSERT_NE(nullptr, strs) << "String datatype not found on multidecorate packet";
  ASSERT_NE(nullptr, strs.get()[0]) << "No strings attached to a multidecorate packet as expected";
  ASSERT_NE(nullptr, strs.get()[1]) << "Expected two strings back, got one";
  ASSERT_EQ(nullptr, strs.get()[2]) << "Expected two strings back, got three";
  ASSERT_EQ("Hello", *strs.get()[0]) << "Entry in multidecorate set was not the expected value";
  ASSERT_EQ("Crickets", *strs.get()[1]) << "Entry in multidecorate set was not the expected value";

  int nEntries;
  ASSERT_NO_THROW(nEntries = packet->Get<int>()) << "Multidecorate filter was not run as expected";
  ASSERT_EQ(2, nEntries) << "Mismatch of number of multi-decorate entries on the packet";
}
