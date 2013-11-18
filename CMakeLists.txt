include_directories(
    ../googletest
    ../googletest/include
    ..
  )

set(
  Autowiring_SRCS
  at_exit.h
  AutoFactory.h
  AutoPacket.h
  AutoPacket.cpp
  AutoPacketFactory.h
  AutoPacketFactory.cpp
  AutoPacketProfiler.h
  AutoPacketProfiler.cpp
  AutoNetworkMonitor.h
  Autowired.h
  Autowired.cpp
  AutowirableSlot.h
  AutowirableSlot.cpp
  autowiring_error.h
  ContextCreator.h
  ContextCreatorBase.h
  ContextCreatorBase.cpp
  Bolt.h
  BoltBase.h
  ContextMap.h
  ContextMember.cpp
  ContextMember.h
  CoreContext.cpp
  CoreContext.h
  CoreThread.cpp
  CoreThread.h
  CurrentContextPusher.cpp
  CurrentContextPusher.h
  Decompose.h
  DeferredBase.h
  DeferredBase.cpp
  DeferredCreationNotice.h
  DeferredCreationNotice.cpp
  deferred_ptr.h
  DispatchQueue.h
  DispatchQueue.cpp
  DispatchThunkEventProxy.h
  DispatchThunkEventProxy.cpp
  EventDispatcher.h
  EventReceiver.h
  EventReceiver.cpp
  EventSender.h
  EventSender.cpp
  ExceptionFilter.h
  FilterPropertyExtractor.h
  GlobalCoreContext.cpp
  GlobalCoreContext.h
  InterlockedExchange.h
  LockFreeList.h
  LockReducedCollection.h
  Object.h
  ObjectPool.h
  PolymorphicTypeForest.h
  ReentrantCounter.h
  ReentrantCounter.cpp
  SharedPtrHash.h
  TeardownNotifier.h
  TeardownNotifier.cpp
  TransientContextMember.h
  TransientPool.h
  TransientPoolBase.h
)

OPTION(BUILD_AUTONET "Enable autonet features of autowiring project." OFF)

if(BUILD_AUTONET)
  set(Autowiring_SRCS
    AutoNetworkMonitor.cpp
    ${Autowiring_SRCS}
  )
endif()

if(BUILD_WINDOWS)
  enable_language(ASM_MASM)
  set(Autowiring_SRCS
	interlocked.asm
    CoreThreadWin.cpp
	InterlockedExchangeWin.cpp
    ${Autowiring_SRCS}
  )
else()
  set(Autowiring_SRCS
    CoreThreadUnix.cpp
	InterlockedExchangeUnix.cpp
    ${Autowiring_SRCS}
  )
endif()

set(
  AutowiringTest_SRCS
  ContextCleanupTest.h
  ContextCleanupTest.cpp
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
  GraphConstructionTest.h
  GraphConstructionTest.cpp
  InterlockedRoutinesTest.h
  InterlockedRoutinesTest.cpp
  LockFreeListTest.h
  LockFreeListTest.cpp
  LockReducedCollectionTest.h
  LockReducedCollectionTest.cpp
  MultiInheritTest.h
  MultiInheritTest.cpp
  ObjectPoolTest.h
  ObjectPoolTest.cpp
  PolymorphicTypeForestTest.h
  PolymorphicTypeForestTest.cpp
  PostConstructTest.h
  PostConstructTest.cpp
  ReentrantCounterTest.h
  ReentrantCounterTest.cpp
  TeardownNotifierTest.h
  TeardownNotifierTest.cpp
  TransientContextMemberTest.h
  TransientContextMemberTest.cpp
  ScopeTest.h
  ScopeTest.cpp
  SnoopTest.h
  SnoopTest.cpp
  TestFixtures/ExitRaceThreaded.h
  TestFixtures/SimpleInterface.h
  TestFixtures/SimpleObject.h
  TestFixtures/SimpleThreaded.h
  TestFixtures/MultiInherit.h
)

set(
  AutowiringFixture_SRCS
  SelfSelectingFixture.h
  SelfSelectingFixture.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" Autowiring_SRCS)
add_library(Autowiring STATIC ${Autowiring_SRCS})
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

  if(NOT BUILD_MAC)
    target_link_libraries(Autowiring -lrt)
  endif()
  target_link_libraries(Autowiring -lpthread ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_CHRONO_LIBRARY})
endif()

if(BUILD_TESTING)
  ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutowiringFixture_SRCS)
  add_library(AutowiringFixture ${AutowiringFixture_SRCS})

  ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutowiringTest_SRCS)
  add_executable(AutowiringTest ${AutowiringTest_SRCS} "../gtest-all-guard.cpp")
  target_link_libraries(AutowiringTest Autowiring AutowiringFixture)
endif(BUILD_TESTING)

if(BUILD_AUTONET)
    add_definitions(-USEAUTONET)
endif()