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
  ContextCreator<sc_pipelineName, std::wstring> pipelineContext;

  // Trivial context creation check:
  std::shared_ptr<CoreContext> ctxt1 = pipelineContext.Create(L"Simple");

  // Now try to autowire a listener:
  AutoRequired<Listener> myListener;

  // Create a second context, verify that the listener got the message:
  std::shared_ptr<CoreContext> ctxt2 = pipelineContext.Create(L"Simple2");

  // Check the listener to verify we had a hit:
  EXPECT_TRUE(myListener->hit) << "The listener callback was not hit as expected";
}