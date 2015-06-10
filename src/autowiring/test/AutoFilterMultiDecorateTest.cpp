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
  for (auto* pCur = desc.GetAutoFilterInput(); *pCur; pCur++)
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
  *f += [](std::string* args[], int& val) {
    for (val = 0; *args; args++, val++);
  };

  auto packet = f->NewPacket();
  packet->Decorate(Decoration<0>{});
  packet->Unsatisfiable<Decoration<1>>();

  const std::string** strs = packet->GetAll<std::string>();
  ASSERT_NE(nullptr, strs) << "String datatype not found on multidecorate packet";
  ASSERT_NE(nullptr, strs[0]) << "No strings attached to a multidecorate packet as expected";
  ASSERT_EQ(nullptr, strs[1]) << "Expected one string back, got two";
  ASSERT_EQ("Hello", *strs[0]) << "Entry in multidecorate set was not the expected value";

  int nEntries;
  ASSERT_NO_THROW(nEntries = packet->Get<int>()) << "Multidecorate filter was not run as expected";
  ASSERT_EQ(1, nEntries) << "Mismatch of number of multi-decorate entries on the packet";
}
