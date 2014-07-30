#include "stdafx.h"
#include "AutoNetServerTest.hpp"
#include "AutoNetServer.h"
#include "Autowired.h"
#include THREAD_HEADER

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

class ExposedAutoNetServer:
  public AutoNetServer
{
public:
  using AutoNetServer::HandleResumeFromBreakpoint;
};

class BreakpointThread:
  public BasicThread
{
  void Run(void) override {
    Autowired<AutoNetServer> autonet;
    autonet->Breakpoint("InThread");
    std::cout << "Thread finished" << std::endl;
  }
};

class WaitsThenSimulatesResume:
  public BasicThread
{
  void Run(void) override {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    AutowiredFast<AutoNetServer> autonet;
    auto autonetInternal = static_cast<ExposedAutoNetServer*>(autonet.get());
    autonetInternal->HandleResumeFromBreakpoint("Main");
    autonetInternal->HandleResumeFromBreakpoint("InThread");
  }
};

TEST_F(AutoNetServerTest, Breakpoint) {
  AutoCurrentContext()->Initiate();
  AutoRequired<AutoNetServer> autonet;
  AutoRequired<BreakpointThread> thread;
  AutoRequired<WaitsThenSimulatesResume>();
  
  autonet->Breakpoint("Main");
}

TEST_F(AutoNetServerTest, SimpleTest) {
  AutoGlobalContext global;
  auto ctxt = global->Create<ContextA>();
  CurrentContextPusher pshr(ctxt);

  AutoRequired<AutoNetServer> server;
  ctxt->Initiate();

  auto ctxt2 = ctxt->Create<ContextB>();
  auto ctxt3 = ctxt->Create<ContextC>();

  ctxt2->Initiate();

  std::shared_ptr<CoreContext> newContext;

  {
    CurrentContextPusher pshr(ctxt2);
    AutoRequired<TestThread1> foo;
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
    *foo += std::chrono::seconds(12), [&ctxt]{
      ctxt->SignalShutdown();
    };
  }

  {
    CurrentContextPusher pshr(ctxt3);
    AutoRequired<TestThread2> bar;
  }

  ctxt->Wait();
}
