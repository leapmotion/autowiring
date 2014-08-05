// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include <autowiring/Autowired.h>
#include <autowiring/AutoNetServer.h>
#include <iostream>

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

  // This will wait indefinitly untill you manual quit the example
  ctxt->Wait();
}