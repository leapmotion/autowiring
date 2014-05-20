include_directories(
    ../googletest
    ../googletest/include
    ..
  )

set(Autowiring_SRCS
  AnySharedPointer.h
  at_exit.h
  AnySharedPointer.h
  AutoAnchor.h
  AutoFuture.h
  AutoFuture.cpp
  CoreJob.h
  CoreJob.cpp
  AutoInjectable.h
  AutoPacket.h
  AutoPacket.cpp
  AutoPacketFactory.h
  AutoPacketFactory.cpp
  AutoPacketListener.h
  AutoPacketProfiler.h
  AutoPacketProfiler.cpp
  AutoPacketSubscriber.h
  Autowired.h
  Autowired.cpp
  AutowirableSlot.h
  AutowirableSlot.cpp
  AutowiringEvents.h
  autowiring_error.h
  BasicThread.h
  BasicThread.cpp
  BasicThreadStateBlock.h
  BasicThreadStateBlock.cpp
  Bolt.h
  BoltBase.h
  ContextCreator.h
  ContextCreatorBase.h
  ContextCreatorBase.cpp
  ContextEnumerator.h
  ContextEnumerator.cpp
  ContextMap.h
  ContextMember.cpp
  ContextMember.h
  CoreContext.cpp
  CoreContext.h
  CoreContextStateBlock.h
  CoreContextStateBlock.cpp
  CoreThread.cpp
  CoreThread.h
  CoreRunnable.h
  CreationRules.h
  CurrentContextPusher.cpp
  CurrentContextPusher.h
  Decompose.h
  Deserialize.h
  deferred_ptr.h
  DispatchQueue.h
  DispatchQueue.cpp
  DispatchThunk.h
  EventInputStream.h
  EventOutputStream.h
  EventOutputStream.cpp
  EventReceiver.h
  EventReceiver.cpp
  fast_pointer_cast.h
  JunctionBox.h
  JunctionBox.cpp
  JunctionBoxEntry.h
  JunctionBoxManager.h
  JunctionBoxManager.cpp
  ExceptionFilter.h
  FilterPropertyExtractor.h
  GlobalCoreContext.cpp
  GlobalCoreContext.h
  is_any.h
  InterlockedExchange.h
  InvokeRelay.h
  Object.h
  ObjectPool.h
  ObjectPoolMonitor.h
  ObjectPoolMonitor.cpp
  move_only.h
  MicroBolt.h
  ReentrantCounter.h
  ReentrantCounter.cpp
  SharedPointerSlot.h
  TeardownNotifier.h
  TeardownNotifier.cpp
  ThreadStatusBlock.h
  ThreadStatusBlock.cpp
  TypeRegistry.h
  TypeRegistry.cpp
  TypeUnifier.h
  uuid.h
)

if(NOT BUILD_MAC)
  # avoid warning: has no symbols
  set(Autowiring_SRCS
    ${Autowiring_SRCS}
    uuid.cpp
  )
endif()

OPTION(BUILD_AUTONET "Enable autonet features of autowiring project." OFF)

set(Autowiring_Win_SRCS
  CoreThreadWin.cpp
  InterlockedExchangeWin.cpp
)

set(Autowiring_Unix_SRCS
	CoreThreadUnix.cpp
	InterlockedExchangeUnix.cpp
)

ADD_MSVC_DISABLED_FILES("Unix Source" Autowiring_SRCS ${Autowiring_Unix_SRCS})

if(BUILD_WINDOWS)
 set(Autowiring_SRCS
	  ${Autowiring_Win_SRCS}
    ${Autowiring_SRCS}
  )

  if(!BUILD_64_BIT)
    enable_language(ASM_MASM)
    set(Autowiring_SRCS interlocked.asm ${Autowiring_SRCS} )
  endif()

else()
  set(Autowiring_SRCS
    ${Autowiring_Unix_SRCS}
    ${Autowiring_SRCS}
  )
endif()

set(AutowiringTest_SRCS
  AnySharedPointerTest.h
  AnySharedPointerTest.cpp
  AutoAnchorTest.h
  AutoAnchorTest.cpp
  AutoInjectableTest.h
  AutoInjectableTest.cpp
  AutoPacketFactoryTest.h
  AutoPacketFactoryTest.cpp
  AutowiringBenchmarkTest.h
  AutowiringBenchmarkTest.cpp
  AutowiringTest.h
  AutowiringTest.cpp
  CoreContextTest.h
  CoreContextTest.cpp
  ContextCleanupTest.h
  ContextCleanupTest.cpp
  ContextEnumeratorTest.h
  ContextEnumeratorTest.cpp
  ContextMapTest.h
  ContextMapTest.cpp
  CoreThreadTest.h
  CoreThreadTest.cpp
  BoltTest.h
  BoltTest.cpp
  ContextCreatorTest.h
  ContextCreatorTest.cpp
  CurrentContextPusherTest.h
  CurrentContextPusherTest.cpp
  DecoratorTest.h
  DecoratorTest.cpp
  DispatchQueueTest.h
  DispatchQueueTest.cpp
  DtorCorrectnessTest.h
  DtorCorrectnessTest.cpp
  EnclosedContextTestBase.h
  ExceptionFilterTest.h
  ExceptionFilterTest.cpp
  EventReceiverTest.h
  EventReceiverTest.cpp
  FactoryTest.h
  FactoryTest.cpp
  GlobalInitTest.h
  GlobalInitTest.cpp
  InterlockedRoutinesTest.h
  InterlockedRoutinesTest.cpp
  MarshalingTest.h
  MarshalingTest.cpp
  MultiInheritTest.h
  MultiInheritTest.cpp
  ObjectPoolTest.h
  ObjectPoolTest.cpp
  PeerContextTest.h
  PeerContextTest.cpp
  PostConstructTest.h
  PostConstructTest.cpp
  ReentrantCounterTest.h
  ReentrantCounterTest.cpp
  SelfSelectingFixtureTest.h
  SelfSelectingFixtureTest.cpp
  TeardownNotifierTest.h
  TeardownNotifierTest.cpp
  CoreJobTest.h
  CoreJobTest.cpp
  TypeRegistryTest.h
  TypeRegistryTest.cpp
  ScopeTest.h
  ScopeTest.cpp
  SnoopTest.h
  SnoopTest.cpp
  TestFixtures/custom_exception.h
  TestFixtures/ExitRaceThreaded.h
  TestFixtures/SimpleInterface.h
  TestFixtures/SimpleObject.h
  TestFixtures/SimpleThreaded.h
  TestFixtures/ThrowingListener.h
  TestFixtures/ThrowsWhenFired.h
  TestFixtures/ThrowsWhenRun.h
  TestFixtures/MultiInherit.h
  UuidTest.h
  UuidTest.cpp
)

set(
  AutowiringFixture_SRCS
  SelfSelectingFixture.h
  SelfSelectingFixture.cpp
  OtherSelectingFixture.h
  OtherSelectingFixture.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" Autowiring_SRCS)
add_library(Autowiring STATIC ${Autowiring_SRCS})

set_property(TARGET Autowiring PROPERTY FOLDER "Autowiring")

if(NOT BUILD_WINDOWS)
  set(Boost_SYSTEM_LIBRARY libboost_system)
  add_library(${Boost_SYSTEM_LIBRARY} STATIC IMPORTED)
  set_property(TARGET ${Boost_SYSTEM_LIBRARY} PROPERTY IMPORTED_LOCATION ${Boost_LIBRARY_DIRS}/${Boost_SYSTEM_LIBRARY}${STATIC_LIBRARY_SUFFIX})

  set(Boost_THREAD_LIBRARY libboost_thread)
  add_library(${Boost_THREAD_LIBRARY} STATIC IMPORTED)
  set_property(TARGET ${Boost_THREAD_LIBRARY} PROPERTY IMPORTED_LOCATION ${Boost_LIBRARY_DIRS}/${Boost_THREAD_LIBRARY}${STATIC_LIBRARY_SUFFIX})

  set(Boost_CHRONO_LIBRARY libboost_chrono)
  add_library(${Boost_CHRONO_LIBRARY} STATIC IMPORTED)
  set_property(TARGET ${Boost_CHRONO_LIBRARY} PROPERTY IMPORTED_LOCATION ${Boost_LIBRARY_DIRS}/${Boost_CHRONO_LIBRARY}${STATIC_LIBRARY_SUFFIX})

  if(Boost_VERSION GREATER 105200)
    set(Boost_ATOMIC_LIBRARY libboost_atomic)
    add_library(${Boost_ATOMIC_LIBRARY} STATIC IMPORTED)
    set_property(TARGET ${Boost_ATOMIC_LIBRARY} PROPERTY IMPORTED_LOCATION ${Boost_LIBRARY_DIRS}/${Boost_ATOMIC_LIBRARY}${STATIC_LIBRARY_SUFFIX})
  endif()

  if(NOT BUILD_MAC AND NOT BUILD_ANDROID)
    # Linux non-Android only
    target_link_libraries(Autowiring -lrt)
  endif()
  if(NOT BUILD_ANDROID)
    target_link_libraries(Autowiring -lpthread)
  endif()
  target_link_libraries(Autowiring ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_CHRONO_LIBRARY})
  if(Boost_VERSION GREATER 105200)
    target_link_libraries(Autowiring ${Boost_ATOMIC_LIBRARY})
  endif()
endif()

if(BUILD_TESTING)
  ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutowiringFixture_SRCS)
  add_library(AutowiringFixture ${AutowiringFixture_SRCS})
  set_property(TARGET AutowiringFixture PROPERTY FOLDER "Autowiring")

  ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutowiringTest_SRCS)
  add_executable(AutowiringTest ${AutowiringTest_SRCS} "../gtest-all-guard.cpp")
  target_link_libraries(AutowiringTest Autowiring AutowiringFixture)
  set_property(TARGET AutowiringTest PROPERTY FOLDER "Autowiring")
endif(BUILD_TESTING)

if(BUILD_AUTONET)
    add_definitions(-USEAUTONET)
endif()
