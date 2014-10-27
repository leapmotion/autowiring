// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoNetServer.h"
#include "Autowired.h"
#include THREAD_HEADER

class BreakpointTest:
  public testing::Test
{};

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

TEST_F(BreakpointTest, SimplePauseAndResume) {
  AutoCurrentContext()->Initiate();
  AutoRequired<AutoNetServer> autonet;
  AutoRequired<BreakpointThread> thread;
  AutoRequired<WaitsThenSimulatesResume>();
  
  autonet->Breakpoint("Main");
}
