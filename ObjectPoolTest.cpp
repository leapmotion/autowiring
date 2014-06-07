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

    ASSERT_TRUE(nullptr != obj1.get()) << "Failed to obtain an entry from a new object pool";

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

TEST_F(ObjectPoolTest, ClearCachedEntities) {
  ObjectPool<PooledObject> pool(3);

  // Create a pool and get a few items in its cache:
  pool.Wait(),
  pool.Wait(),
  pool.Wait();

  // Verify the expected initial cache count:
  ASSERT_EQ(3UL, pool.GetCached()) << "Expected pool outstanding count to have hit 3 entries";

  // Now verify that we can clear the object pool at a point in time when it should already be empty:
  pool.ClearCachedEntities();

  // And we should be legitimately empty at this point
  ASSERT_EQ(0UL, pool.GetCached()) << "After invoking a cache clearing operation, the cache was nevertheless not cleared";
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

class HoldsSharedPtrThenQuits:
  public CoreThread
{
public:
  std::shared_ptr<int> m_ptr;

  void Run(void) override {
    ThreadSleep(boost::chrono::milliseconds(100));
    m_ptr.reset();
  }
};

TEST_F(ObjectPoolTest, EmptyPoolIssuance) {
  ObjectPool<int> pool;

  // Create the thread which will hold the shared pointer for awhile:
  AutoRequired<HoldsSharedPtrThenQuits> thread;
  pool(thread->m_ptr);
  std::weak_ptr<int> ptrWeak = thread->m_ptr;

  ASSERT_FALSE(ptrWeak.expired()) << "Object pool failed to issue a shared pointer as expected";

  // Verify properties now that we've zeroized the limit:
  pool.SetOutstandingLimit(0);
  EXPECT_ANY_THROW(pool.SetOutstandingLimit(1)) << "An attempt to alter a zeroized outstanding limit did not throw an exception as expected";
  EXPECT_ANY_THROW(pool.Wait()) << "An attempt to obtain an element on an empty pool did not throw an exception as expected";

  // Now see if we can delay for the thread to back out:
  m_create->Initiate();
  pool.Rundown();

  // Verify that it got released as expected:
  ASSERT_TRUE(ptrWeak.expired()) << "Not all shared pointers issued by an object pool expired in a timely fashion";
}

TEST_F(ObjectPoolTest, CanRundownOneIssued) {
  // No conditions to be checked, we just know these routines should not deadlock.
  ObjectPool<int> pool;
  pool.Wait();
  pool.Rundown();
}

TEST_F(ObjectPoolTest, OutstandingLimitIsOne) {
  ObjectPool<int> pool(1);
  AutoRequired<CoreThread> ct;
  AutoCurrentContext()->Initiate();

  // Bounce 1000 shared pointers into the CoreThread
  for(size_t i = 0; i < 1000; i++) {
    // Try to wait on the pool:
    auto entity = pool.WaitFor(boost::chrono::seconds(10));
    ASSERT_NE(nullptr, entity) << "Failed to obtain an entity from a single-element pool";

    // Pend a lambda that just holds a closure on the entity:
    *ct += [entity] {};
  }
}

TEST_F(ObjectPoolTest, MovableObjectPool) {
  ObjectPool<int> to;

  // Verify a move works correctly when an object has been checked out:
  {
    ObjectPool<int> from;
    auto original = from.Wait();
    to = std::move(from);
  }

  // Now verify that the pooled object returned home to the right place:
  ASSERT_EQ(1UL, to.GetCached()) << "Object pool move operation did not correctly relay checked out types";
}

TEST_F(ObjectPoolTest, MovableObjectPoolAysnc) {
  static const size_t sc_count = 10000;
  ObjectPool<int> from;

  {
    // Issue a zillion objects from the from pool:
    std::vector<std::shared_ptr<int>> objs;
    for(size_t i = sc_count; i--;)
      objs.push_back(from.Wait());

    // Make a thread, let it hold these objects while we move its pool:
    *AutoRequired<CoreThread>() += [objs] {};
  }

  // Kick off threads, then immediately and asynchronously move the pool:
  AutoCurrentContext()->Initiate();
  ObjectPool<int> to = std::move(from);

  // Shutdown time:
  AutoCurrentContext()->SignalShutdown(true);

  // Verify that new pool got all of the objects:
  ASSERT_EQ(sc_count, to.GetCached()) << "Object pool move operation did not correctly relay checked out types";
}