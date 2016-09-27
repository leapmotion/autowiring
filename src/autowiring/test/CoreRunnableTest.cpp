// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
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

  bool OnStart(void) override {
    m_myContext->Initiate();
    return false;
  }
  
  void OnStop(bool) override {
    m_myContext.reset();
  }
};

TEST_F(CoreRunnableTest, CanStartSubcontextWhileInitiating) {
  AutoRequired<StartsSubcontextWhileStarting>();
  AutoCurrentContext()->Initiate();
}

TEST_F(CoreRunnableTest, InnerContextInitiateNoRecurse) {
  AutoCurrentContext ctxt;

  // Create and initiate a child context
  AutoCreateContext child;
  child->Initiate();

  // Verify state
  ASSERT_FALSE(ctxt->IsInitiated()) << "Parent context incorrectly initiated as a result of a child context being initiated";
  ASSERT_FALSE(child->IsRunning()) << "Parent context not initiated yet, child context should not be considered running";

  // Now start the outer context, verify the child context transitions to the "running" state
  ctxt->Initiate();
  ASSERT_TRUE(child->IsRunning()) << "Child context did not transition to the running state when the parent context was initiated";
}
