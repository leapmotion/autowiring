#include "stdafx.h"
#include "GuardObjectTest.h"
#include "shared_object.h"
#include "unlock_object.h"

TEST_F(GuardObjectTest, SharedTests) {
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

TEST_F(GuardObjectTest, UnlockTests) {
  shared_object<int> so1; //Default Constructor
  ASSERT_FALSE(so1.initialized()); //default initialization test
  {
    unlock_object<int> so1_unlock1(so1); //successful unlock
    ASSERT_TRUE(static_cast<bool>(so1_unlock1));

    unlock_object<int> so1_unlock2(so1, true); //prevented try_unlock
    ASSERT_FALSE(static_cast<bool>(so1_unlock2));

    *so1_unlock1 = 1; //object assignment
  } //delete one empty and one held unlock_object instance
  int val = 0;
  ASSERT_TRUE(so1.initialized(val)); //condition for unlock_object construction
  ASSERT_TRUE(val == 1);

  shared_object<int> so2(2); //Default Constructor
  {
    unlock_object<int> so_unlock3(so1); //successful unlock
    ASSERT_TRUE(static_cast<bool>(so_unlock3));
    ASSERT_TRUE(*so_unlock3 == 1);

    so_unlock3.reset(so2); //reset with argument
    ASSERT_TRUE(static_cast<bool>(so_unlock3));
    ASSERT_TRUE(*so_unlock3 == 2);

    so_unlock3.reset(so2); //reset is idempotent
    ASSERT_TRUE(static_cast<bool>(so_unlock3));
    ASSERT_TRUE(*so_unlock3 == 2);
  }
}
