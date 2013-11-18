#include "stdafx.h"
#include "SelfSelectingFixtureTest.h"
#include "SelfSelectingFixture.h"

TEST_F(SelfSelectingFixtureTest, FixtureTest) {
  // Create a context with the fixture test name:
  auto created = AutoCurrentContext()->Create(s_expectedFixtureName);
  ASSERT_TRUE(created != nullptr) << "Created context was unexpectedly null";

  // Verify that the context has the name we gave to it:
  ASSERT_EQ(0, strcmp(s_expectedFixtureName, created->GetName())) << "Context was incorrectly named";

  // Set the current context and detect the SelfSelectingFixture's presence
  CurrentContextPusher pshr(created);
  Autowired<SelfSelectingFixture> ssf;
  ASSERT_TRUE(ssf.IsAutowired()) << "Self-selecting fixture was not selected into a created context with the correct name";
  ASSERT_TRUE(ssf->IsMagicCorrect()) << "Self-selecting fixture was apparently not initialized correctly";
}