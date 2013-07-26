#include "stdafx.h"
#include "ObjectPoolTest.h"
#include "ObjectPool.h"

class PooledObject {
};

TEST_F(ObjectPoolTest, VerifyOutstandingLimit) {
  ObjectPool<PooledObject> pool(2);

  std::shared_ptr<PooledObject> obj1, obj2, obj3;

  // Try to grab some objects out of the pool:
  pool(obj1);
  pool(obj2);

  // Verify that grabbing a third object fails:
  pool(obj3);
  EXPECT_TRUE(obj3 == nullptr) << "Object pool issued more objects than it was authorized to issue";
}
