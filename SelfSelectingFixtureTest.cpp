#include "stdafx.h"
#include "SelfSelectingFixtureTest.h"
#include "SelfSelectingFixture.h"
#include "MicroBolt.h"

TEST_F(SelfSelectingFixtureTest, FixtureTest) {
  // Create a context with the fixture test name:
  auto created = AutoCurrentContext()->Create(s_expectedFixtureName);
  ASSERT_TRUE(created != nullptr) << "Created context was unexpectedly null";

  // Verify that the context has the name we gave to it:
  ASSERT_NE(nullptr, created->GetName()) << "Name was empty, should have held the context name";
  ASSERT_EQ(0, strcmp(s_expectedFixtureName, created->GetName())) << "Context was incorrectly named";

  // Set the current context and detect the SelfSelectingFixture's presence
  CurrentContextPusher pshr(created);
  Autowired<SelfSelectingFixture> ssf;
  ASSERT_TRUE(ssf.IsAutowired()) << "Self-selecting fixture was not selected into a created context with the correct name";
  ASSERT_TRUE(ssf->IsMagicCorrect()) << "Self-selecting fixture was apparently not initialized correctly";
}

TEST_F(SelfSelectingFixtureTest, PostHocBoltIntroduction) {
  // Create a context FIRST, and THEN create the bolt:
  CurrentContextPusher pshr(m_create->Create("SelfSelect3"));
  Autowired<SelfSelectingFixture> ssf;
  ASSERT_FALSE(ssf.IsAutowired()) << "Fixture self-selected into a context prematurely";

  // Now the bolt, this should introduce the fixture into the already-existing context
 // BOLT_TO(SelfSelectingFixture, "Global/IAMANON/SelfSelect3");
  
  // Autowired fields are funny--they can be satisfied at the point of their declaration, or if a member
  // is added to a context which might satisfy an autowired field _after_ it is declared, they can be
  // satisfied later.  This is called delay-satisfaction and it's necessary that bolted introductions of
  // context members honor this property.
  ASSERT_TRUE(ssf.IsAutowired());
}