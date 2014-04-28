#include "stdafx.h"
#include "FactoryTest.h"
#include "TestFixtures/SimpleInterface.h"
#include <iostream>

using namespace std;

/// <summary>
/// Compile-time check to ensure that unconstructable types are identified correctly
/// </summary>
class ClassWithIntegralCtor
{
public:
  ClassWithIntegralCtor(int) {}
};
static_assert(!has_simple_constructor<ClassWithIntegralCtor>::value, "A class without a simple constructor was incorrectly identified as having one");


class ClassWithStaticNew:
  public Object
{
public:
  ClassWithStaticNew(bool madeByFactory = false):
    m_madeByFactory(madeByFactory)
  {}

  const bool m_madeByFactory;

  // Factory method, for trivial factory construction:
  static ClassWithStaticNew* New(void) {
    return new ClassWithStaticNew(true);
  }
};

TEST_F(FactoryTest, VerifyFactoryCall) {
  static_assert(has_simple_constructor<ClassWithStaticNew>::value, "Class with default-argument constructor was not correctly detected as such ");
  static_assert(has_static_new<ClassWithStaticNew>::value, "Class with static allocator was not correctly detected as having one");

  // Try to create the static new type:
  AutoRequired<ClassWithStaticNew> si;

  // Verify the correct version was called:
  ASSERT_TRUE(si->m_madeByFactory) << "A factory method was not called on a type which provided a static factory New method";
}
