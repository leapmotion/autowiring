#include "stdafx.h"
#include "CtorProxyTest.h"
#include "CurrentContextPusher.h"
#include "Autowired.h"

// Simple class and embedded class factory, for testing purposes:
class SimpleClass {
public:
  SimpleClass(void) :
    m_i(1)
  {}

  int m_i;

  static SimpleClass* Create(void) {
    return new SimpleClass;
  }
};

TEST_F(CtorProxyTest, VerifySimple) {
  std::weak_ptr<SimpleClass> weak;
  {
    std::shared_ptr<CoreContext> context = CoreContext::CurrentContext()->Create();
    CurrentContextPusher psher(context);

    // Trivial creation and destruction test:
    {
      AutoRequired<
        CtorProxy<SimpleClass, &SimpleClass::Create>
      > req;
      ASSERT_TRUE(req.IsAutowired()) << "Class factory AutoRequired didn't create as expected";

      weak = req;
      EXPECT_EQ(1, req->m_i) << "Class factory didn't invoke the SimpleClass ctor";
    }
    EXPECT_FALSE(weak.expired()) << "CtorProxy object was prematurely free";
  }
  EXPECT_TRUE(weak.expired()) << "CtorProxy object leaked from its parent context";
}