// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/autowiring.h>
#include <autowiring/AutowiringDebug.h>
#include "TestFixtures/Decoration.hpp"
#include <algorithm>
#include <fstream>

class AutowiringDebugTest:
  public testing::Test
{};

TEST_F(AutowiringDebugTest, IsLambdaTest) {
  auto fn = [] {};
  ASSERT_TRUE(autowiring::dbg::IsLambda(typeid(fn))) << "Lambda function not correctly detected";
  ASSERT_FALSE(autowiring::dbg::IsLambda(typeid(CoreThread))) << "Simple class incorrectly identified as a lambda function";
  ASSERT_FALSE(autowiring::dbg::IsLambda(typeid(std::vector<int>))) << "Template class incorrectly identified as a lambda function";
}

class HasDebugInformation {
public:
  void AutoFilter(const Decoration<0>& dec, Decoration<1>&) {}
};

TEST_F(AutowiringDebugTest, FilterInfoTest) {
  AutoRequired<HasDebugInformation> hdi;

  auto text = autowiring::dbg::AutoFilterInfo("HasDebugInformation");
  ASSERT_NE(std::string{"Filter not found"}, text) << "Debug helper routine did not find a named filter in the current context as expected";
}

TEST_F(AutowiringDebugTest, IdentifyRootType) {
  AutoRequired<AutoPacketFactory> factory;

  *factory += [](const Decoration<0>&, Decoration<1>&) {};
  *factory += [](const Decoration<1>&, Decoration<2>&) {};

  auto entries = autowiring::dbg::ListRootDecorations();
  ASSERT_EQ(1UL, entries.size()) << "An incorrect number of unsatisfied decorations was detected";
  ASSERT_EQ("Decoration<0>", entries[0]) << "The wrong decoration was detected as being unsatisfiable";
}

TEST_F(AutowiringDebugTest, CanGetCurrentPacket) {
  AutoCurrentContext()->Initiate();
  AutoRequired<AutoPacketFactory> factory;
  *factory += [](AutoPacket& packet) {
    ASSERT_EQ(&packet, autowiring::dbg::CurrentPacket()) << "CurrentPacket did not correctly report the current packet";
  };

  auto packet = factory->NewPacket();
}

struct Herp{};
struct Derp{};

TEST_F(AutowiringDebugTest, ContextPrintout) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  AutoCreateContextT<Derp> dCtxt;
  auto ctxt1 = dCtxt->Create<Herp>();
  auto ctxt2 = dCtxt->Create<Derp>();
  auto ctxt3 = ctxt1->Create<int>();
  AutoCreateContextT<Herp> hCtxt;
  auto ctxt4 = hCtxt->Create<Derp>();
  auto ctxt5 = hCtxt->Create<Derp>();

  // This is the expected output
  std::string tree = "GlobalCoreContext\n"
                     "└── void(Current Context)\n"
                     "    ├── Derp\n"
                     "    │   ├── Herp\n"
                     "    │   │   └── int\n"
                     "    │   └── Derp\n"
                     "    └── Herp\n"
                     "        ├── Derp\n"
                     "        └── Derp\n";

  // Remove whitespace so test is more robust
  tree.erase(std::remove_if(tree.begin(), tree.end(), [](char ch) { return ch == ' '; }), tree.end());

  // Write output to stringstream an remove whitespace
  std::stringstream ss;
  autowiring::dbg::PrintContextTree(ss);
  auto output = ss.str();
  output.erase(std::remove_if(output.begin(), output.end(), [](char ch) { return ch == ' '; }), output.end());

  ASSERT_EQ(tree, output) << "Didn't print correct tree";
}

struct IntOutputer {
  void AutoFilter(int& i) {
    i = 4;
  }
};

struct IntInFloatOut {
  void AutoFilter(const int& i, float& f) {
    f = static_cast<float>(i) + 3;
  }
};

struct IntInFloatIn {
  void AutoFilter(const int& i, const float& f) {}
};

TEST_F(AutowiringDebugTest, BasicAutoFilterGraph) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<IntOutputer> filter1;
  AutoRequired<IntInFloatOut> filter2;
  AutoRequired<IntInFloatIn> filter3;

  *factory += [](auto_prev<float> in, std::string& out) {
    out = "hello world";
  };

  *factory += [](double& out) { out = 3.0;};
  *factory += [](double& out) { out = 4.0; };

  *factory += [](const double* vals []){};

  AutoCurrentContext()->Initiate();
  auto packet = factory->NewPacket();

  std::stringstream ss;
  autowiring::dbg::WriteAutoFilterGraph(ss);
}
