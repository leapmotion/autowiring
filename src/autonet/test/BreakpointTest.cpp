// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "src/autonet/AutoNetServerImpl.hpp"
#include "Autowired.h"
#include THREAD_HEADER

class BreakpointTest:
  public testing::Test
{};

class ExposedAutoNetServer:
  public AutoNetServerImpl<websocketpp::config::asio>
{
public:
  void HandleResumeFromBreakpoint(const std::string& breakpoint) {
    *this += [this, breakpoint] {
      for (const auto& handler : this->m_handlers["resumeFromBreakpoint"]){
        std::vector<std::string> args{breakpoint};
        handler(args);
      }
    };
  }
};

class BreakpointThread:
  public BasicThread
{
  void Run(void) override {
    Autowired<ExposedAutoNetServer> autonet;
    autonet->Breakpoint("InThread");
    std::cout << "Thread finished" << std::endl;
  }
};

class WaitsThenSimulatesResume:
  public BasicThread
{
  void Run(void) override {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    AutowiredFast<ExposedAutoNetServer> autonet;
    autonet->HandleResumeFromBreakpoint("Main");
    autonet->HandleResumeFromBreakpoint("InThread");
  }
};

TEST_F(BreakpointTest, SimplePauseAndResume) {
  AutoCurrentContext()->Initiate();
  AutoRequired<ExposedAutoNetServer> autonet;
  AutoRequired<BreakpointThread> thread;
  AutoRequired<WaitsThenSimulatesResume>();
  
  autonet->Breakpoint("Main");
}
