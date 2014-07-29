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
  
  ctxt->Wait();

  std::shared_ptr<CoreContext> newContext;

  {
    CurrentContextPusher pshr(ctxt2);
    AutoRequired<TestThread1> foo;
    *foo += std::chrono::seconds(4),[&ctxt]{
      ctxt->Inject<ThisClassGetsAddedLater<4>>();
    };
    *foo += std::chrono::seconds(6),[&ctxt]{
      ctxt->Inject<ThisThreadGetsAddedLater<6>>();
    };
    *foo += std::chrono::seconds(8),[&ctxt2]{
      AutoRequired<ThisClassGetsAddedLater<8>> derp(ctxt2);
    };
    *foo += std::chrono::seconds(12),[&ctxt3]{
      AutoRequired<ThisClassGetsAddedLater<12>> derp(ctxt3);
    };
    *foo += std::chrono::seconds(14),[&ctxt3]{
      AutoRequired<ThisThreadGetsAddedLater<14>> derp(ctxt3);
    };
    *foo += std::chrono::seconds(16),[&ctxt]{
      AutoRequired<ThisClassGetsAddedLater<16>> derp(ctxt);
    };
    *foo += std::chrono::seconds(18),[&newContext, &ctxt2]{
      newContext = ctxt2->Create<TestThread1>();
    };
    *foo += std::chrono::seconds(20),[&ctxt2]{
      AutoRequired<ThisClassGetsAddedLater<20>> derp(ctxt2);
    };
    *foo += std::chrono::seconds(24),[&ctxt3]{
      AutoRequired<ThisClassGetsAddedLater<24>> derp(ctxt3);
    };
    *foo += std::chrono::seconds(28),[&ctxt]{
      AutoRequired<ThisClassGetsAddedLater<28>> derp(ctxt);
    };
    *foo += std::chrono::seconds(32),[&ctxt2]{
      AutoRequired<ThisClassGetsAddedLater<32>> derp(ctxt2);
    };
    *foo += std::chrono::seconds(34),[&ctxt3]{
      ctxt3->SignalShutdown(true);
      ctxt3.reset();
    };
    *foo += std::chrono::seconds(36),[&ctxt]{
      AutoRequired<ThisClassGetsAddedLater<36>> derp(ctxt);
    };
  }

  {
    CurrentContextPusher pshr(ctxt3);
    AutoRequired<TestThread2> bar;
  }

  ctxt->Wait();
}
