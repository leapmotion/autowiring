// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/SimpleObject.hpp"
#include <autowiring/autowiring.h>
#include <autowiring/AutoRestarter.h>
#include <autowiring/CoreThread.h>

class AutoRestarterTest : public testing::Test {};

class RestartingSigil {};

class CreationDetectionBolt:
  public Bolt<RestartingSigil>
{
public:
  void ContextCreated(void) override {
    called = true;
    nContextsCreated++;
  }

  size_t nContextsCreated = 0;
  bool called = false;
};

class ThrowsAnExceptionFirstTime:
  public CoreThread
{
public:
  void Run(void) override {
    AutowiredFast<CreationDetectionBolt> cdb;
    if(cdb)
      if(!cdb->nContextsCreated++)
        throw std::runtime_error("Crashing for no reason!");
  }
};

TEST_F(AutoRestarterTest, RestarterCanExistInStoppedContext) {
  AutoRestarterConfig cfg;
  cfg.startWhenCreated = true;
  AutoConstruct<AutoRestarter<RestartingSigil>> restarter(cfg);
}

TEST_F(AutoRestarterTest, RestartsOnException) {
  AutoCurrentContext()->Initiate();

  // Use a bolt facility to attach one of our text fixtures:
  AutoCurrentContext()->BoltTo<ThrowsAnExceptionFirstTime, RestartingSigil>();

  // Create our bolt and the restarter
  AutoRequired<CreationDetectionBolt> bolt;

  AutoRestarterConfig cfg;
  cfg.restartOnException = true;
  cfg.restartOnShutdown = false;
  cfg.startWhenCreated = true;
  AutoConstruct<AutoRestarter<RestartingSigil>> restarter(cfg);

  // Verify the bolt got called:
  ASSERT_TRUE(bolt->called) << "Bolt was not called even though a context restarter was present in the current context";

  // Verify subcontext properties:
  auto subCtxt = restarter->GetContext();
  ASSERT_TRUE(subCtxt != nullptr) << "Restarter did not correctly create a subcontext";
  ASSERT_TRUE(subCtxt->Is<RestartingSigil>()) << "Generated subcontext was not marked with the right sigil";
  ASSERT_TRUE(subCtxt->IsInitiated()) << "Generated subcontext should have been prospectively started, but was not";

  // Terminate the subcontext directly:
  subCtxt->SignalShutdown();

  // Verify that this causes the restarter to release its hold on the subcontext:
  auto newSubCtxt = restarter->GetContext();
  ASSERT_NE(subCtxt, newSubCtxt) << "Restarter did not release a terminated subcontext";
  ASSERT_FALSE(newSubCtxt) << "Restarter should not have attempted to create any new contexts on teardown";
}

TEST_F(AutoRestarterTest, RestartsOnShutdown) {
  AutoCurrentContext()->Initiate();

  // Create the restarter
  AutoRestarterConfig cfg;
  cfg.restartOnShutdown = true;
  cfg.startWhenCreated = true;
  AutoConstruct<AutoRestarter<RestartingSigil>> restarter(cfg);

  // Terminate the restarter's subcontext:
  auto subCtxt = restarter->GetContext();
  subCtxt->SignalShutdown();

  // New context should be created immediately
  ASSERT_NE(subCtxt, restarter->GetContext()) << "Restarter incorrectly held original context beyond shutdown";
  ASSERT_TRUE(restarter->GetContext() != nullptr) << "Restarter did not correctly generate a new context after termination";
}
