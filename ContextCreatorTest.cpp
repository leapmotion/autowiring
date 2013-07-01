#include "stdafx.h"
#include "ContextCreatorTest.h"
#include "ContextCreator.h"
#include <string>

extern const char gc_contextName[] = "eviction_context";

class Creator:
  public ContextCreator<gc_contextName, std::string> {
};

TEST_F(ContextCreatorTest, ValidateSimpleEviction) {
  // Create a context and verify it gets evicted from the context creator:
  AutoRequired<Creator> creator;
  std::weak_ptr<CoreContext> ctxtWeak;

  {
    std::shared_ptr<CoreContext> ctxt;

    // Make a context:
    ctxt = *creator->CreateContext("test");

    // Obtain a weak pointer, in order to ensure proper teardown:
    ctxtWeak = ctxt;
  }

  // Context must be destroyed as a precondition of the subsequent assertion
  ASSERT_TRUE(ctxtWeak.expired()) << "Expected the context to be destroyed";

  // Verify that our creator is now empty:
  EXPECT_EQ(0UL, creator->EvictStale()) << "Unexpected stale context found";
}