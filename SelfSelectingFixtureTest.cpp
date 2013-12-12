#include "stdafx.h"
#include "SelfSelectingFixtureTest.h"
#include "SelfSelectingFixture.h"
#include "MicroBolt.h"
#include "UnutteredSelectingFixture.h"

// Simple class we will bolt locally in this application
class SimpleLocalClass {};

TEST_F(SelfSelectingFixtureTest, LocalFixtureTest) {
}

TEST_F(SelfSelectingFixtureTest, ExteriorFixtureTest) {
  // Enable our self selectors:
  AutoEnable<SelfSelectingFixture>();
  AutoEnable<UnutteredSelectingFixture>();

  // Create a context with the fixture test name:
  auto created = AutoCurrentContext()->Create<SelfSelect>();
  ASSERT_TRUE(created != nullptr) << "Created context was unexpectedly null";

  // Verify that the context has the name we gave to it:
  ASSERT_EQ(typeid(SelfSelect), created->GetSigilType()) << "Context was incorrectly named";

  // Set the current context and detect the SelfSelectingFixture's presence
  CurrentContextPusher pshr(created);
  Autowired<SelfSelectingFixture> ssf;
  Autowired<UnutteredSelectingFixture> usf;

  ASSERT_TRUE(ssf.IsAutowired()) << "Self-selecting fixture was not selected into a created context with the correct name";
  ASSERT_TRUE(ssf->IsMagicCorrect()) << "Self-selecting fixture was apparently not initialized correctly";

  // Verify that both bolts, including the one we shall not name, made it in:
  ASSERT_EQ(2UL, created->GetMemberCount()) << "An unuttered fixture was eliminated by the linker unexpectedly";
}

