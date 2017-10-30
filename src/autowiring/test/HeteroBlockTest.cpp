// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/Decoration.hpp"
#include <autowiring/HeteroBlock.h>

using autowiring::HeteroBlock;
using autowiring::HeteroBlockEntry;

class HeteroBlockTest:
  public testing::Test
{};

TEST_F(HeteroBlockTest, ObjectOrganizationTest) {
  HeteroBlock block{
    HeteroBlockEntry { auto_id_t<Decoration<0>>(), 0 },
    HeteroBlockEntry { auto_id_t<Decoration<0>>(), 1 },
    HeteroBlockEntry { auto_id_t<Decoration<0>>(), 2 }
  };

  // Obtain one of each of the known entries:
  auto& obj0 = block.get<Decoration<0>>(0);
  auto& obj1 = block.get<Decoration<0>>(1);
  auto& obj2 = block.get<Decoration<0>>(2);
  ASSERT_LT(&obj0, &obj1);
  ASSERT_LT(&obj1, &obj2);
}

TEST_F(HeteroBlockTest, SharedPtrAliasTest) {
  HeteroBlock block{
    { auto_id_t<Decoration<0>>(), 0 },
    { auto_id_t<Decoration<0>>(), 1 },
    { auto_id_t<Decoration<0>>(), 2 }
  };

  // Obtain one of each of the known entries:
  auto& obj0 = block.get<Decoration<0>>(0);
  auto obj0Shared = block.get_shared<Decoration<0>>(0);
  auto& obj1 = block.get<Decoration<0>>(1);
  auto obj1Shared = block.get_shared<Decoration<0>>(1);
  auto& obj2 = block.get<Decoration<0>>(2);
  auto obj2Shared = block.get_shared<Decoration<0>>(2);
  ASSERT_EQ(&obj0, obj0Shared.get());
  ASSERT_EQ(&obj1, obj1Shared.get());
  ASSERT_EQ(&obj2, obj2Shared.get());
}

struct HeteroBlockTest_DestructorValidationTest {
  HeteroBlockTest_DestructorValidationTest(void) {}

  std::shared_ptr<bool> destroyed;
};

TEST_F(HeteroBlockTest, DestroyObjTest) {
  std::shared_ptr<bool> ref;
  {
    HeteroBlock block{
      { auto_id_t<HeteroBlockTest_DestructorValidationTest>(), 0 },
      { auto_id_t<HeteroBlockTest_DestructorValidationTest>(), 1 },
      { auto_id_t<HeteroBlockTest_DestructorValidationTest>(), 2 }
    };
    auto& obj0 = block.get<HeteroBlockTest_DestructorValidationTest>(0);
    obj0.destroyed = std::make_shared<bool>(false);
    ref = obj0.destroyed;
  }
  ASSERT_TRUE(ref.unique()) << "Object referred to by a shared pointer was not destroyed as expected";
}

TEST_F(HeteroBlockTest, DestroySharedPtrTest) {
  std::shared_ptr<bool> ref;
  {
    HeteroBlock block {
      { auto_id_t<HeteroBlockTest_DestructorValidationTest>(), 0 }
    };
    auto& ptr0 = block.get_shared<HeteroBlockTest_DestructorValidationTest>(0);
    ptr0->destroyed = std::make_shared<bool>(false);
    ref = ptr0->destroyed;
  }
  ASSERT_TRUE(ref.unique()) << "Object referred to by a shared pointer was not destroyed as expected";
}
class AUTO_ALIGNAS(16) MyAlignedClass{
  int v;
};

TEST_F(HeteroBlockTest, StackAlignmentCheck) {
  MyAlignedClass a;
  MyAlignedClass b;
  MyAlignedClass c;

  ASSERT_EQ(0UL, reinterpret_cast<size_t>(&a) % 16) << "Misalignment of variable A";
  ASSERT_EQ(0UL, reinterpret_cast<size_t>(&b) % 16) << "Misalignment of variable B";
  ASSERT_EQ(0UL, reinterpret_cast<size_t>(&c) % 16) << "Misalignment of variable C";
}
