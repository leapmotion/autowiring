// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include <autowiring/autowiring.h>
#include <autonet/AutoNetServer.h>
#include <iostream>
#include THREAD_HEADER

//
// AutoNetServer
//
// This example creates a sample context structure to view with the
// AutoNetVisualizer. It creates contexts and adds dummy context members
// at timed intervals to show off the dynamic nature of the visualizer.
// You can view the visualizer at leapmotion.github.io/autonet
//

//
// Declaration of dummy classes to view in the visualizer
//

class TestThread1:
  public CoreThread
{};

class TestThread2:
  public CoreThread
{};

template<int T>
class ThisClassGetsAddedLater:
  public ContextMember
{};

template<int T>
class ThisThreadGetsAddedLater:
  public CoreThread
{};

template<int N>
class TestData {
public:
  int value;
  TestData(int n = N): value(n) {}
};

template<typename... Args>
class TestAutoFilter:
  public ContextMember //Needed in order to be displayed by AutoNet
{
public:
  void AutoFilter(Args... a) {}
};

class ContextA {};
class ContextB {};
class ContextC {};


int main() {
  AutoGlobalContext global;
  auto ctxt = global->Create<ContextA>();
  CurrentContextPusher pshr(ctxt);

  // To use the visualizer, you just need to AutoRequire the AutoNetServer
  AutoRequired<AutoNetServer> server;

  // Initiate context to start threads
  ctxt->Initiate();

  // Create a bunch of example Contexts and Context Members
  auto ctxt2 = ctxt->Create<ContextB>();
  auto ctxt3 = ctxt->Create<ContextC>();

  ctxt2->Initiate();

  {
    CurrentContextPusher pshr(ctxt3);
    AutoRequired<TestThread2> bar;
  }

  std::shared_ptr<CoreContext> newContext;

  {
    CurrentContextPusher pshr(ctxt2);
    AutoRequired<TestThread1> foo;

    AutoRequired<TestAutoFilter<const TestData<0>&, TestData<1>&>> filter01;
    AutoRequired<TestAutoFilter<const TestData<1>&, TestData<2>&>> filter12;

    // Give time to open AutoNet Visualizer
    std::this_thread::sleep_for(std::chrono::seconds(10));

    *foo += std::chrono::seconds(1),[&ctxt]{
      ctxt->Inject<ThisClassGetsAddedLater<4>>();
    };
    *foo += std::chrono::seconds(2),[&ctxt]{
      ctxt->Inject<ThisThreadGetsAddedLater<6>>();
    };
    *foo += std::chrono::seconds(3),[&ctxt2]{
      AutoRequired<ThisClassGetsAddedLater<8>> derp(ctxt2);
    };
    *foo += std::chrono::seconds(4),[&ctxt3]{
      AutoRequired<ThisClassGetsAddedLater<12>> derp(ctxt3);
    };
    *foo += std::chrono::seconds(5),[&ctxt3]{
      AutoRequired<ThisThreadGetsAddedLater<14>> derp(ctxt3);
    };
    *foo += std::chrono::seconds(6),[&ctxt]{
      AutoRequired<ThisClassGetsAddedLater<16>> derp(ctxt);
    };
    *foo += std::chrono::seconds(7),[&newContext, &ctxt2]{
      newContext = ctxt2->Create<TestThread1>();
    };
    *foo += std::chrono::seconds(8),[&ctxt2]{
      AutoRequired<ThisClassGetsAddedLater<20>> derp(ctxt2);
    };
    *foo += std::chrono::seconds(9),[&ctxt3]{
      AutoRequired<ThisClassGetsAddedLater<24>> derp(ctxt3);
    };
    *foo += std::chrono::seconds(10),[&ctxt3]{
      ctxt3->SignalShutdown(true);
      ctxt3.reset();
    };
  }

  // This will wait indefinitly until you manually quit the example
  ctxt->Wait();
}
