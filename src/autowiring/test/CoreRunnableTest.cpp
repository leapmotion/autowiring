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

  bool DoStart(void) override {
    m_myContext->Initiate();
    return false;
  }
};

TEST_F(CoreRunnableTest, CanStartSubcontextWhileInitiating) {
  AutoRequired<StartsSubcontextWhileStarting>();
  AutoCurrentContext()->Initiate();
}