// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/Autowired.h>
#include <autowiring/GlobalCoreContext.h>
#include <autowiring/TypeRegistry.h>

class TypeRegistryTest:
  public testing::Test
{};

template<int>
class Registered{};

TEST_F(TypeRegistryTest, VerifySimpleLocalRegistration) {
  // Register two entries statically by uttering the static member's name:
  (void)RegType<Registered<1>>::r;
  (void)RegType<Registered<2>>::r;

  // Verify that both types exist:
  bool exists1 = false;
  bool exists2 = false;

  // Enumerate, skipping both bounds on this set.  Note that the set will contain ALL utterances
  // of RegType, which means we must enumerate everything, and have to expect a lot of negative
  // matches.
  for(auto p = g_pFirstTypeEntry; p; p = p->pFlink) {
    if(p->GetTypeInfo() == typeid(Registered<1>))
      exists1 = true;
    if(p->GetTypeInfo() == typeid(Registered<2>))
      exists2 = true;

    // Negative assertion:
    ASSERT_NE(p->ti, typeid(Registered<3>)) << "Obtained a registration for a type which was not explicitly registered";
  }

  ASSERT_TRUE(exists1) << "Type one was registered but not found";
  ASSERT_TRUE(exists2) << "Type two was registered but not found";
}

TEST_F(TypeRegistryTest, VerifyExteriorModuleRegistration) {
  // Verify that there are a bunch of registered types in the test application by looking
  // at the length of the linked list.  We expect that there will be at least 2, because of
  // our declarations in the earlier test in this module, but we also expect there to be
  // (to choose arbitrarily) around 8, because this is a number of autofired fields known
  // to exist at one point.
  //
  // If the number of autowired members in this unit test drops below 10 at some point, it
  // might be necessary to reduce the count from 8 to something lower.
  size_t nTypes = 0;

  for(auto p = g_pFirstTypeEntry; p; p = p->pFlink)
    nTypes++;

  ASSERT_LT(8UL, nTypes) << "Registration failed to pick up the expected minimum number of types in this test";
  ASSERT_EQ(g_typeEntryCount, nTypes) << "Linked list did not contain the same number of entries as reported in g_entryCount";
}
