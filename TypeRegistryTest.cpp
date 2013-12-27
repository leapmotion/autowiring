#include "stdafx.h"
#include "TypeRegistryTest.h"
#include "TypeRegistry.h"

template<int>
class Registered {};

TEST_F(TypeRegistryTest, VVV) {
  // Register two entries statically by uttering the static member's name:
  RegType<Registered<1>>::r;
  RegType<Registered<2>>::r;

  // Verify that both types exist:
  bool exists1 = false;
  bool exists2 = false;

  // Enumerate, skipping both bounds on this set.  Note that the set will contain ALL utterances
  // of RegType, which means we must enumerate everything, and have to expect a lot of negative
  // matches.
  for(auto p = g_pFirstEntry; p; p = p->pFlink) {
    if(p->ti == typeid(Registered<1>))
      exists1 = true;
    if(p->ti == typeid(Registered<2>))
      exists2 = true;

    // Negative assertion:
    ASSERT_NE(p->ti, typeid(Registered<3>)) << "Obtained a registration for a type which was not explicitly registered";
  }

  ASSERT_TRUE(exists1) << "Type one was registered but not found";
  ASSERT_TRUE(exists2) << "Type two was registered but not found";
}