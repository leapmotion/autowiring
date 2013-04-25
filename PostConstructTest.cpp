#include "stdafx.h"
#include "PostConstructTest.h"
#include "Autowired.h"
#include "ContextMember.h"

using namespace std;

class ContextExposer:
  public CoreContext
{
public:
  size_t DeferredCount(void) const {return CoreContext::m_deferred.size();}
};

// Two classes to make up the cyclic dependency:
class A:
  public ContextMember
{
public:
  A(void):
    m_value(2)
  {}

  int m_value;

  int GetValue(void) const {return m_value;}
};

class Naive:
  public ContextMember
{
public:
  Naive(void) {
    if(!m_a)
      throw exception();
  }

  Autowired<A> m_a;
};

class Smarter:
  public ContextMember
{
public:
  Smarter(void):
    value(1)
  {
#if LAMBDAS_AVAILABLE
    m_a.NotifyWhenAutowired([this] () {
      this->value = m_a->GetValue();
    });
#endif
  }

  int value;
  Autowired<A> m_a;
};

TEST_F(PostConstructTest, VerifyNaiveBehavior) {
  // Create a context and add just the naive class, to verify the problematic behavior:
  cpp11::shared_ptr<CoreContext> subContext = CoreContext::CurrentContext()->Create();
  EXPECT_THROW(subContext->Add<Naive>(), std::exception) << "Naive class didn't throw an exception as expected";
}

TEST_F(PostConstructTest, VerifyExpectedDeferrmentCount) {
  cpp11::shared_ptr<CoreContext> subContext = CoreContext::CurrentContext()->Create();
  CurrentContextPusher cur(subContext);

  // Add the smart class, which should introduce a single deferred count:
  subContext->Add<Smarter>();

  // Now test the count:
  EXPECT_EQ(
    1,
    ((ContextExposer&)*subContext).DeferredCount()
  ) << "Unexpected number of deferred initializers";
}

TEST_F(PostConstructTest, VerifySmartBehavior) {
  // These tests are only eabled where C++11 is present, because they are somewhat complicated to implement without it.
  if(!LAMBDAS_AVAILABLE)
    return;

  cpp11::shared_ptr<CoreContext> subContext = CoreContext::CurrentContext()->Create();
  CurrentContextPusher cur(subContext);

  // Add the smart class, which should allow 
  subContext->Add<Smarter>();

  // Initially, value should be one, which is the default
  Autowired<Smarter> smarter;
  EXPECT_EQ(1, smarter->value) << "Unexpected initial value of Smarter instance";

  // Now we add A and check the wiring
  subContext->Add<A>();
  EXPECT_FALSE(!smarter->m_a.get()) << "Autowired member was not wired as expected";

  // Verify the value was updated by the notification routine
  EXPECT_EQ(2, smarter->value) << "Post-construction notification routine wasn't invoked as expected";
}
