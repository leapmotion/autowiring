#include "stdafx.h"
#include "SharedGuardObjectTest.h"
#include "shared_object.h"
#include "guard_object.h"

TEST_F(SharedGuardObjectTest, SharedTests) {
  shared_object<int> so1; //Default Constructor
  ASSERT_FALSE(so1.initialized()); //default initialization test

  int val = 0;
  so1 = 1; //object assignment
  ASSERT_TRUE(so1.initialized(val)); //atomic assignment & initialization test
  ASSERT_TRUE(val == 1);

  val = 2;
  shared_object<int> so2(val); //object copy constructor
  val = 1;
  ASSERT_TRUE(so2.initialized(val));
  ASSERT_TRUE(val == 2);

  so1.reset(); //reset
  ASSERT_FALSE(so1.initialized());
  so1.reset(); //reset is idempotent

  so1 = so2; //shared_object assignment
  ASSERT_TRUE(so1.initialized());
  ASSERT_TRUE(so1 == 2);

  so2 = 3;
  shared_object<int> so3(so2); //shared_object copy constructor
  ASSERT_TRUE(so3.initialized());
  val = so3; //implicit cast to copy
  ASSERT_TRUE(val == 3);

  so1 = so3; //shared_object assignment
  val = 1;
  ASSERT_TRUE(so1.initialized(val));
  ASSERT_TRUE(val == 3);
}

TEST_F(SharedGuardObjectTest, GuardTests) {
  shared_object<int> so1; //Default Constructor
  ASSERT_FALSE(so1.initialized()); //default initialization test

  {
    guard_object<int> so1_hold1 = so1.try_hold(); //successful try_hold
    ASSERT_TRUE(static_cast<bool>(so1_hold1));

    guard_object<int> so1_hold2 = so1.try_hold(); //prevented try_hold
    ASSERT_FALSE(static_cast<bool>(so1_hold2));

    so1_hold2 = so1_hold1; //guard_object assignment
    ASSERT_TRUE(static_cast<bool>(so1_hold2));

    so1_hold1.reset(); //reset
    ASSERT_FALSE(so1_hold1);
    so1_hold1.reset(); //reset is idempotent

    guard_object<int> so1_hold3(so1_hold2); //copy constructor
    *so1_hold2 = 1; //object assignment
  } //delete one empty and two equal guard_object instances
  int val = 0;
  ASSERT_TRUE(so1.initialized(val)); //condition for guard_object construction
  ASSERT_TRUE(val == 1);
}
