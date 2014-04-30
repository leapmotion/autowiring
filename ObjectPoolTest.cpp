#include "stdafx.h"
#include "ObjectPoolTest.h"
#include "ObjectPool.h"
#include "TestFixtures/SimpleThreaded.h"

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

TEST_F(ObjectPoolTest, VerifyAsynchronousUsage) {
  AutoCreateContext ctxt;
  CurrentContextPusher pshr(ctxt);
  
  AutoRequired<SimpleThreadedT<PooledObject>> obj;
  AutoFired<SharedPtrReceiver<PooledObject>> spr;
  ObjectPool<PooledObject> pool(3);

  {
    // Obtain the pool limit in objects:
    std::shared_ptr<PooledObject> obj1, obj2, obj3;
    pool(obj1);
    pool(obj2);
    pool(obj3);
    
    // Block--verify that we _do not_ get any of those objects back while they are
    // still outstanding.
    {
      auto obj4 = pool.WaitFor(boost::chrono::milliseconds(1));
      EXPECT_TRUE(obj4 == nullptr) << "Pool issued another element even though it should have hit its outstanding limit";
    }
    
    // Now we kick off threads:
    AutoCurrentContext()->Initiate();

    // Fire off a few events:
    spr(&SharedPtrReceiver<PooledObject>::OnEvent)(obj1);
    spr(&SharedPtrReceiver<PooledObject>::OnEvent)(obj2);
    spr(&SharedPtrReceiver<PooledObject>::OnEvent)(obj3);
  }

  // This should return more or less right away as objects become available:
  {
    auto obj4 = pool.WaitFor(boost::chrono::milliseconds(10));
    EXPECT_TRUE(obj4 != nullptr) << "Object pool failed to be notified that it received a new element";
  }

  // Cause the thread to quit:
  *obj += [&obj] { obj->Stop(); };
  obj->Wait();
}

TEST_F(ObjectPoolTest, VerifyOutOfOrderDestruction) {
  std::shared_ptr<int> ptr;

  {
    ObjectPool<int> pool;
    pool(ptr);
  }

  // Verify that returning a shared pointer after the pool is gone does not result in an exception
  ASSERT_NO_THROW(ptr.reset()) << "Attempting to release a shared pointer on a destroyed pool caused an unexpected exception";
}
