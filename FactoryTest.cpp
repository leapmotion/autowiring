// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "FactoryTest.h"
#include "AutoFactory.h"
#include "TestFixtures/SimpleInterface.h"
#include <iostream>

using namespace std;

TEST_F(FactoryTest, VerifyFactoryCall) {
  class ClassWithStaticNew
  {
  public:
    ClassWithStaticNew(bool madeByFactory = false):
      m_madeByFactory(madeByFactory)
    {}

    bool m_madeByFactory;

    // Factory method, for trivial factory construction:
    static ClassWithStaticNew* New(void) {
      return new ClassWithStaticNew(true);
    }
  };
  static_assert(has_simple_constructor<ClassWithStaticNew>::value, "Class with default-argument constructor was not correctly detected as such ");
  static_assert(has_static_new<ClassWithStaticNew>::value, "Class with static allocator was not correctly detected as having one");

  // Try to create the static new type:
  AutoRequired<ClassWithStaticNew> si;

  // Verify the correct version was called:
  ASSERT_TRUE(si->m_madeByFactory) << "A factory method was not called on a type which provided a static factory New method";
}


/// <summary>
/// Compile-time check to ensure that unconstructable types are identified correctly
/// </summary>
class ClassWithIntegralCtor:
  public SimpleInterface
{
public:
  ClassWithIntegralCtor(int) {}

  void Method(void) override {}
};
static_assert(!has_simple_constructor<ClassWithIntegralCtor>::value, "A class without a simple constructor was incorrectly identified as having one");


TEST_F(FactoryTest, VerifyCanRequireAbstract) {
  /// <summary>
  /// A factory for SimpleInterface
  /// </summary>
  class SimpleInterfaceFactory:
    public AutoFactory<SimpleInterface>
  {
  public:
    SimpleInterface* New(void) {return new ClassWithIntegralCtor(1);}
  };

  // Insert the factory type into the context:
  AutoRequired<SimpleInterfaceFactory> factory;

  // Now request that the factory be used to create a new SimpleInterface type
  Autowired<SimpleInterface> si;
  ASSERT_TRUE(si.IsAutowired()) << "Autowiring a type for which a factory exists did not correctly result in the construction of that type";

  // Verify that the type of the constructed item is what we expect, too:
  ASSERT_EQ(typeid(ClassWithIntegralCtor), typeid(*si)) << "The factory-constructed type was not the type the factory should have constructed.";
}