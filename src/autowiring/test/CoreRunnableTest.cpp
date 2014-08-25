// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/autowiring.h>
#include <autowiring/CoreRunnable.h>

class CoreRunnableTest:
  public testing::Test
{};

class StartsSubcontextWhileStarting:
  public CoreRunnable
{
public:
  AutoCreateContext m_myContext;

  bool Start(std::shared_ptr<Object> outstanding) override {
    m_myContext->Initiate();
    return true;
  }

  void Stop(bool graceful) override {}
  bool IsRunning(void) const override { return false; }
  bool ShouldStop(void) const override { return true; }
  void Wait(void) override {}
};

TEST_F(CoreRunnableTest, CanStartSubcontextWhileInitiating) {
  AutoRequired<StartsSubcontextWhileStarting>();
  AutoCurrentContext()->Initiate();
}