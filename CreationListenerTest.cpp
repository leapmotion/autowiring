#include "stdafx.h"
#include "CreationListenerTest.h"
#include "Autowired.h"
#include "ContextCreationListener.h"
#include "ContextCreator.h"
#include <string>

extern const char sc_pipelineName[] = "Pipeline";

class Listener:
  public ContextCreationListener<sc_pipelineName>
{
public:
  Listener(void):
    hit(false)
  {}

  bool hit;

  void ContextCreated(const std::shared_ptr<CoreContext>& context) override {
    hit = true;
  }
};

class Creator:
  public ContextCreator<sc_pipelineName, std::string>
{
};

TEST_F(CreationListenerTest, VerifyMapping) {
  ContextCreator<sc_pipelineName, std::wstring> simpleCreator;

  // Trivial context creation check:
  {
    std::shared_ptr<CoreContext> ctxt1 = simpleCreator.Create(L"Simple");
    EXPECT_TRUE(!!ctxt1.get()) << "Initial context creation did not succeed as expected";
  }

  // Now try to autowire a listener:
  AutoRequired<Listener> myListener;

  // Create a second context, verify that the listener got the message:
  std::shared_ptr<CoreContext> ctxt2 = simpleCreator.Create(L"Simple2");

  // Check the listener to verify we had a hit:
  EXPECT_TRUE(myListener->hit) << "The listener callback was not hit as expected";
}

TEST_F(CreationListenerTest, VerifyDescendantMapping) {
  // Leakage check:
  {
    Autowired<Listener> validation;
    ASSERT_FALSE(validation.IsAutowired());
  }

  // Put the creator in the parent context:
  AutoRequired<ContextCreator<sc_pipelineName, std::wstring>> simpleCreator;
  std::shared_ptr<Listener> listener;

  // Create a child context of the current one, and put the listener in there:
  AutoCreateContext childContext;
  {
    CurrentContextPusher pshr(childContext);
    AutoRequired<Listener> myListener;
    listener = myListener;
  }

  // Dependent context broadcast check:
  simpleCreator->Create(L"Simple");

  // Check the listener to verify we had a hit:
  EXPECT_TRUE(listener->hit) << "The listener callback was not hit as expected";
}