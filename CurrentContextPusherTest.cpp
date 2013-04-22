#include "stdafx.h"
#include "CurrentContextPusherTest.h"
#include "CoreContext.h"
#include "CurrentContextPusher.h"

using namespace std;

TEST_F(CurrentContextPusherTest, VerifyResetsDuringThrow) {
  cpp11::shared_ptr<CoreContext> current = CoreContext::CurrentContext();

  try {
    cpp11::shared_ptr<CoreContext> subContext = current->Create();
    CurrentContextPusher test(subContext.get());
    throw exception();
  } catch(exception&) {
  }

  ASSERT_EQ(current.get(), CoreContext::CurrentContext().get());
}