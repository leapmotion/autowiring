// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "SelfSelectingFixtureTest.hpp"
#include "SelfSelectingFixture.hpp"
#include "OtherSelectingFixture.hpp"
#include <autowiring/MicroBolt.h>

struct SelfSelect {};

// Simple class we will bolt locally in this application
class SimpleLocalClass {};

// Bolt which just counts hits:
class HitCountingBolt:
  public ContextMember,
  public Bolt<SimpleLocalClass>
{
public:
  HitCountingBolt(void):
    m_hitCount(0)
  {}

  void ContextCreated(void) override {
    m_hitCount++;
  }

  size_t m_hitCount;
};

TEST_F(SelfSelectingFixtureTest, LocalFixtureTest) {
  AutoCurrentContext ctxt;
  Autowired<HitCountingBolt> hcb;
  ASSERT_FALSE(hcb.IsAutowired()) << "Hit-counting bolt was created before it was enabled";

  // Enable the bolt
  AutoEnable<HitCountingBolt>();

  // Verify creation:
  ASSERT_TRUE(hcb.IsAutowired()) << "Bolt not created after being enabled";

  // Verify the bolt gets hit:
  ctxt->Create<SimpleLocalClass>();
  ASSERT_EQ(1UL, hcb->m_hitCount) << "Bolt was not hit when a matching class was created";
}

TEST_F(SelfSelectingFixtureTest, ExteriorFixtureTest) {
  // Enable our self selectors:
  AutoEnable<SelfSelectingFixture>();
  AutoEnable<OtherSelectingFixture>();

  // Create a context with the fixture test name:
  auto created = AutoCurrentContext()->Create<SelfSelect>();
  ASSERT_TRUE(created != nullptr) << "Created context was unexpectedly null";

  // Verify that the context has the name we gave to it:
  ASSERT_EQ(typeid(SelfSelect), created->GetSigilType()) << "Context was incorrectly named";

  // Set the current context and detect the SelfSelectingFixture's presence
  CurrentContextPusher pshr(created);
  Autowired<SelfSelectingFixture> ssf;
  Autowired<OtherSelectingFixture> usf;

  ASSERT_TRUE(ssf.IsAutowired()) << "Self-selecting fixture was not selected into a created context with the correct name";
  ASSERT_TRUE(ssf->IsMagicCorrect()) << "Self-selecting fixture was apparently not initialized correctly";

  // Verify that both bolts, including the one we shall not name, made it in:
  ASSERT_EQ(2UL, created->GetMemberCount()) << "An unuttered fixture was eliminated by the linker unexpectedly";
}
