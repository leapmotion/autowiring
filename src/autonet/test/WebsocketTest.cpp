// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autonet/AutoNetServer.h>
#include <autowiring/autowiring.h>
#include CHRONO_HEADER

#include <iostream>

class WebsocketTest:
  public testing::Test
{};

class WebsocketExceptionFilter:
  public ExceptionFilter
{
public:
  void Filter(void) {
    try {
      throw;
    } catch(std::exception& e) {
      FAIL() << "Problem starting websocket server: " << std::string(e.what());
    }
  };
};

TEST_F(WebsocketTest, CleanShutdown) {
  AutoRequired<WebsocketExceptionFilter>();

  // Try starting and stopping server multiple times
  {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);
    AutoRequired<AutoNetServer>();

    ctxt->Initiate();
    ctxt->Wait(std::chrono::milliseconds(200));
    ctxt->SignalShutdown(true);
  }

  {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);
    AutoRequired<AutoNetServer>();

    ctxt->Initiate();
    ctxt->Wait(std::chrono::milliseconds(200));
    ctxt->SignalShutdown(true);
  }

  {
    AutoCreateContext ctxt;
    CurrentContextPusher pshr(ctxt);
    AutoRequired<AutoNetServer>();

    ctxt->Initiate();
    ctxt->Wait(std::chrono::milliseconds(200));
    ctxt->SignalShutdown(true);
  }
}
