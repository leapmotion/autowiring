#include "stdafx.h"
#include "DependentContextTest.h"
#include "Autowired.h"

class ClassA:
  public ContextMember
{};

class ClassB:
  public ContextMember
{};

class ClassC:
  public ContextMember
{};

struct ContextExample {
  AutoRequired<ClassA> m_classA;
  AutoRequired<ClassB> m_classB;
  AutoRequired<ClassC> m_classC;
};

TEST_F(DependentContextTest, ValidateMembership) {
  cpp11::shared_ptr<CoreContext> context = CoreContext::CurrentContext();
  cpp11::weak_ptr<CoreContext> subContext;

  {
    // Try creating a dependent context:
    cpp11::shared_ptr<DependentContext<ContextExample> > ex(
      context->CreateDependentContext<ContextExample>()
    );

    // Verify the context is what we expect:
    ASSERT_EQ(
      context,
      ex->m_context->GetParentContext()
    ) << "Dependent context didn't have the expected parent";

    // Verify each of the members exist:
    ASSERT_TRUE(ex->m_classA) << "ClassA instance not initialized";
    ASSERT_TRUE(ex->m_classB) << "ClassB instance not initialized";
    ASSERT_TRUE(ex->m_classC) << "ClassC instance not initialized";

    // Copy over the subcontext and ensure there aren't too many outstanding strong references
    subContext = ex->m_context;
    ASSERT_EQ(1, ex->m_context.use_count()) << "Too many outstanding references for a created subcontext";
  }

  ASSERT_TRUE(subContext.expired()) << "Outstanding reference exists for the subcontext";
}