// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/autowiring.h>
#include <autowiring/AutowiringDebug.h>
#include "TestFixtures/Decoration.hpp"
#include <algorithm>
#include <fstream>
#include <thread>

class AutowiringDebugTest:
  public testing::Test
{};

TEST_F(AutowiringDebugTest, IsLambdaTest) {
  auto fn = [] {};
  ASSERT_TRUE(autowiring::dbg::IsLambda(typeid(fn))) << "Lambda function not correctly detected";
  ASSERT_FALSE(autowiring::dbg::IsLambda(typeid(CoreThread))) << "Simple class incorrectly identified as a lambda function";
  ASSERT_FALSE(autowiring::dbg::IsLambda(typeid(std::vector<int>))) << "Template class incorrectly identified as a lambda function";
}

TEST_F(AutowiringDebugTest, IdentifyRootType) {
  AutoRequired<AutoPacketFactory> factory;

  *factory += [](const Decoration<0>&, Decoration<1>&) {};
  *factory += [](const Decoration<1>&, Decoration<2>&) {};
  *factory += [](Decoration<1>&&) {};

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
  std::string tree = "void(Current Context)\n"
                     "|-- Derp\n"
                     "|   |-- Herp\n"
                     "|   |   *-- int\n"
                     "|   *-- Derp\n"
                     "*-- Herp\n"
                     "    |-- Derp\n"
                     "    *-- Derp\n";

  // Remove whitespace so test is more robust
  tree.erase(std::remove_if(tree.begin(), tree.end(), [](char ch) { return ch == ' '; }), tree.end());

  // Write output to stringstream an remove whitespace
  std::stringstream ss;
  autowiring::dbg::PrintContextTree(ss, ctxt);
  auto output = ss.str();
  output.erase(std::remove_if(output.begin(), output.end(), [](char ch) { return ch == ' '; }), output.end());

  ASSERT_STREQ(tree.c_str(), output.c_str()) << "Didn't print correct tree";
}

struct IntOutputer {
  void AutoFilter(int& i) {
    i = 4;
  }
};

struct IntModifier {
  void AutoFilter(int&& i) {
    i = 5;
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

TEST_F(AutowiringDebugTest, FilterInfoTest) {
  AutoCurrentContext()->Initiate();
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<IntOutputer> filter1;
  AutoRequired<IntInFloatOut> filter2;
  AutoRequired<IntInFloatIn> filter3;
  AutoRequired<IntModifier> filter4;

  auto text = autowiring::dbg::AutoFilterInfo("IntInFloatIn");
  ASSERT_NE(std::string{"Filter not found"}, text) << "Debug helper routine did not find a named filter in the current context as expected";
}

TEST_F(AutowiringDebugTest, BasicAutoFilterGraph) {
  AutoRequired<AutoPacketFactory> factory;
  AutoRequired<IntOutputer> filter1;
  AutoRequired<IntInFloatOut> filter2;
  AutoRequired<IntInFloatIn> filter3;
  AutoRequired<IntModifier> filter4;

  *factory += [](double& out) { out = 3.0;};
  *factory += [](double& out) { out = 4.0; };

  *factory += [](const double* vals []){};

  AutoCurrentContext()->Initiate();
  auto packet = factory->NewPacket();

  std::stringstream ss;
  autowiring::dbg::WriteAutoFilterGraph(ss);
}

struct IncompleteOutputType;

class SyntheticFilterIDClass {
public:
  void AutoFilter(std::shared_ptr<IncompleteOutputType>&) {}
};

TEST_F(AutowiringDebugTest, FilterWithSyntheticID) {
  AutoRequired<SyntheticFilterIDClass>();

  std::ostringstream os;
  autowiring::dbg::WriteAutoFilterGraph(os);
  std::string str = os.str();

  size_t off = str.find("IncompleteOutputType [i]", 0);
  ASSERT_NE(std::string::npos, off) << "AutoFilterGraph dump failed to correctly handle an uninstantiated output type";
}

template<int>
class RunnableMember:
  public ContextMember,
  public CoreRunnable
{
public:
  RunnableMember(const char* name) :
    ContextMember(name)
  {}

  bool OnStart(void) override { return true; }
};

TEST_F(AutowiringDebugTest, PrintRunnables) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  ctxt->Inject<RunnableMember<0>>("efgh");

  auto child1 = ctxt->Create<void>();
  child1->Initiate();
  child1->Inject<RunnableMember<0>>("abcd");

  auto child2 = ctxt->Create<void>();
  child2->Initiate();
  child2->Inject<RunnableMember<0>>("efgh");

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  std::stringstream os;
  autowiring::dbg::PrintRunnables(os, *ctxt);
  auto str = os.str();

  ASSERT_EQ(
    "void(Current Context)\n"
    "|   [ RUNNING ] RunnableMember<0>  (efgh)\n"
    "|-- void\n"
    "|       [ RUNNING ] RunnableMember<0>  (abcd)\n"
    "*-- void\n"
    "|       [ RUNNING ] RunnableMember<0>  (efgh)\n",
    str
  );
}

namespace {
class HasNoName :
  public CoreThread
{
public:
  HasNoName(void) :
    CoreThread(nullptr)
  {}
};
}

TEST_F(AutowiringDebugTest, NullContextMemberName) {
  AutoCurrentContext ctxt;
  AutoRequired<HasNoName>{};

  ctxt->Initiate();

  std::stringstream str;
  autowiring::dbg::PrintRunnables(str, *AutoCurrentContext{});

  static const char expected[] = "void(Current Context)\n";
  ASSERT_STREQ(
    expected,
    str.str().substr(0, sizeof(expected) - 1).c_str()
  );
}
