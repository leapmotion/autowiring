// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/SimpleThreaded.hpp"
#include <autowiring/ObjectPool.h>
#include FUTURE_HEADER
#include THREAD_HEADER

class ObjectPoolTest:
  public testing::Test
{};

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
  ASSERT_TRUE(obj3 == nullptr) << "Object pool issued more objects than it was authorized to issue";
}

class LifeCycle {
  static std::atomic<int> constructNum;
  static std::atomic<int> destructNum;

  enum LifeStage {
    pooled,
    issued
  };
  std::atomic<LifeStage> stage;

public:
  LifeCycle():
    stage{pooled}
  {
    ++constructNum;
  }

  ~LifeCycle() {
    ++destructNum;
    if (stage != pooled)
      throw std::runtime_error("Destructor called before Finalize");
  }

  static ObjectPool<LifeCycle>* NewObjectPool(size_t limit = ~0, size_t maxPooled = ~0) {
    return new ObjectPool<LifeCycle>(
      limit,
      maxPooled,
      [] (LifeCycle* ptr) { new (ptr) LifeCycle; },
      [] (LifeCycle& life) { life.Initialize(); },
      [] (LifeCycle& life) { life.Finalize(); }
    );
  }

  static void InitializeNum() {
    constructNum = 0;
    destructNum = 0;
  }
  static int ConstructNum() {
    return constructNum;
  }
  static int DestructNum() {
    return destructNum;
  }

protected:
  void Initialize() {
    if (stage != pooled)
      throw std::runtime_error("Initialize called on object not pooled");
    stage = issued;
  }

  void Finalize() {
    if (stage != issued)
      throw std::runtime_error("Finalize called on object not issued");
    stage = pooled;
  }
};

std::atomic<int> LifeCycle::constructNum{0};
std::atomic<int> LifeCycle::destructNum{0};

TEST_F(ObjectPoolTest, LifeCycleTestLimitOne) {
  LifeCycle::InitializeNum();

  std::shared_ptr<ObjectPool<LifeCycle>> pool(LifeCycle::NewObjectPool(2, 2));
  std::shared_ptr<LifeCycle> objHold, objDrop;

  // Create in Pool
  try {
    pool->Preallocate(1);
  } catch (std::runtime_error e) {
    FAIL() << e.what();
  }
  ASSERT_EQ(1, LifeCycle::ConstructNum());
  ASSERT_EQ(0, LifeCycle::DestructNum());

  // Issue from Pool
  try {
    (*pool)(objHold);
  } catch (std::runtime_error e) {
    FAIL() << e.what();
  }
  ASSERT_EQ(1, LifeCycle::ConstructNum());
  ASSERT_EQ(0, LifeCycle::DestructNum());

  // Issue from Pool with implicit Creation of objDrop
  try {
    (*pool)(objDrop);
  } catch (std::runtime_error e) {
    FAIL() << e.what();
  }
  ASSERT_EQ(2, LifeCycle::ConstructNum());
  ASSERT_EQ(0, LifeCycle::DestructNum());

  // Return to Pool
  try {
    objDrop.reset();
  } catch (std::runtime_error e) {
    FAIL() << e.what();
  }
  ASSERT_EQ(2, LifeCycle::ConstructNum());
  ASSERT_EQ(0, LifeCycle::DestructNum());

  // Destroy Pool with implicit Destruction of objDrop
  try {
    pool.reset();
  } catch (std::runtime_error e) {
    FAIL() << e.what();
  }
  ASSERT_EQ(2, LifeCycle::ConstructNum());
  ASSERT_EQ(1, LifeCycle::DestructNum());

  // Return to Pool redirected to Destruction of objHold
  try {
    objHold.reset();
  } catch (std::runtime_error e) {
    FAIL() << e.what();
  }
  ASSERT_EQ(2, LifeCycle::ConstructNum());
  ASSERT_EQ(2, LifeCycle::DestructNum());
}

TEST_F(ObjectPoolTest, DISABLED_VerifyAsynchronousUsage) {
  AutoCreateContext ctxt;
  CurrentContextPusher pshr(ctxt);

  AutoRequired<SimpleThreadedT<PooledObject>> obj;
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
      auto obj4 = pool.WaitFor(std::chrono::milliseconds(1));
      ASSERT_TRUE(obj4 == nullptr) << "Pool issued another element even though it should have hit its outstanding limit";
    }

    // Now we kick off threads:
    AutoCurrentContext()->Initiate();

    // Pass a few copies
    obj->OnEvent(obj1);
    obj->OnEvent(obj2);
    obj->OnEvent(obj3);
  }

  // This should return more or less right away as objects become available:
  {
    auto obj4 = pool.WaitFor(std::chrono::milliseconds(10));
    ASSERT_TRUE(obj4 != nullptr) << "Object pool failed to be notified that it received a new element";
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
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    m_ptr.reset();
  }
};

TEST_F(ObjectPoolTest, EmptyPoolIssuance) {
  AutoCurrentContext ctxt;
  ObjectPool<int> pool;

  // Create the thread which will hold the shared pointer for awhile:
  AutoRequired<HoldsSharedPtrThenQuits> thread;
  pool(thread->m_ptr);
  std::weak_ptr<int> ptrWeak = thread->m_ptr;

  ASSERT_FALSE(ptrWeak.expired()) << "Object pool failed to issue a shared pointer as expected";

  // Verify properties now that we've zeroized the limit:
  pool.SetOutstandingLimit(0);
  ASSERT_ANY_THROW(pool.SetOutstandingLimit(1)) << "An attempt to alter a zeroized outstanding limit did not throw an exception as expected";
  ASSERT_ANY_THROW(pool.Wait()) << "An attempt to obtain an element on an empty pool did not throw an exception as expected";

  // Now see if we can delay for the thread to back out:
  ctxt->Initiate();
  ASSERT_TRUE(ctxt->IsRunning());
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

class ExposedObjectPool:
  public ObjectPool<int>
{
public:
  using ObjectPool<int>::m_monitor;
};

TEST_F(ObjectPoolTest, RundownWhileWaiting) {
  ObjectPool<int> pool(1);
  auto first = pool.Wait();

  std::mutex lock;
  std::condition_variable cv;
  bool proceed = false;

  auto future = std::async(std::launch::async, [&] {
    std::lock_guard<std::mutex>{ lock },
    proceed = true;
    cv.notify_all();

    // This could throw, but we can't guarantee that it will; in either case,
    // the behavior is tolerated
    try {
      pool.Wait();
    } catch(...) {}
  });

  // Block until the async call is at least started
  std::unique_lock<std::mutex> lk(lock);
  ASSERT_TRUE(
    cv.wait_for(lk, std::chrono::seconds(5), [&proceed] { return proceed; })
  ) << "Dependent thread did not initiate in time";

  // Block until the thread is (probably) waiting
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  // Now, ensure no crashing:
  first.reset();
  pool.Rundown();

  // Make sure the thread goes away before we do:
  future.wait();
}

TEST_F(ObjectPoolTest, OutstandingLimitIsOne) {
  ObjectPool<int> pool(1);
  AutoRequired<CoreThread> ct;
  AutoCurrentContext()->Initiate();

  // Bounce 1000 shared pointers into the CoreThread
  for(size_t i = 0; i < 1000; i++) {
    // Try to wait on the pool:
    auto entity = pool.WaitFor(std::chrono::seconds(10));
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
  static const size_t s_count = 1000;
  ObjectPool<int> from;

  {
    // Issue a zillion objects from the from pool:
    std::vector<std::shared_ptr<int>> objs;
    for(size_t i = s_count; i--;)
      objs.push_back(from.Wait());

    // Make a thread, let it hold these objects while we move its pool:
    *AutoRequired<CoreThread>() += [objs] {};
  }

  ASSERT_EQ(0, from.GetCached()) << "Initial pool received cached entities unexpectedly";

  // Kick off threads, then immediately and asynchronously move the pool:
  AutoCurrentContext()->Initiate();
  ObjectPool<int> to = std::move(from);

  // Shutdown time:
  AutoCurrentContext()->SignalShutdown(true);

  // Verify that new pool got all of the objects:
  ASSERT_EQ(s_count, to.GetCached()) << "Object pool move operation did not correctly relay checked out types";
}

TEST_F(ObjectPoolTest, VerifyInitializerFinalizer) {
  auto initFlag = std::make_shared<bool>(false);
  auto termFlag = std::make_shared<bool>(false);

  ObjectPool<bool> pool(
    autowiring::placement,
    &DefaultPlacement<bool>,
    [initFlag](bool&) { *initFlag = true; },
    [termFlag](bool&) { *termFlag = true; }
  );

  // Issue a shared pointer, this shouldn't invoke the finalizer but should invoke the initializer
  auto issued = pool.Wait();
  ASSERT_TRUE(*initFlag) << "Item issued from a pool was not properly initialized";
  ASSERT_FALSE(*termFlag) << "Item issued from a pool was finalized before it was released";

  // Now return the issued item, and verify that the correct functions get called
  *initFlag = false;
  issued.reset();
  ASSERT_FALSE(*initFlag) << "Returned item incorrectly caused a new initialization";
  ASSERT_TRUE(*termFlag) << "Returned item was not correctly finalized";
}

TEST_F(ObjectPoolTest, PlacementConstructor) {
  ObjectPool<int> pool(
    autowiring::placement,
    [](int* pVal) {
      *pVal = 109;
    },
    [](int& val) {
      ASSERT_EQ(val, 109) << "Value was not placement constructed properly";
      val = 110;
    }
  );

  auto obj = pool();
  ASSERT_EQ(110, *obj) << "Value was not correctly initialized";
}
