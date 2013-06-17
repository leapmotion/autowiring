include_directories(
    ../googletest
    ../googletest/include
    ..
  )

set(
  Autowiring_SRCS
  AutoNetworkMonitor.h
  AutoNetworkMonitor.cpp
  Autowired.h
  Autowired.cpp
  AutowirableSlot.h
  AutowirableSlot.cpp
  Autowirer.cpp
  Autowirer.h
  CMakeLists.txt
  ContextCreator.h
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
  DeferredBase.h
  DeferredBase.cpp
  DeferredCreationNotice.h
  DeferredCreationNotice.cpp
  DependentContext.h
  DispatchQueue.h
  DispatchQueue.cpp
  EventDispatcher.h
  EventSender.h
  EventSender.cpp
  ExceptionFilter.h
  Factory.h
  GlobalCoreContext.cpp
  GlobalCoreContext.h
  InterlockedExchange.h
  LockReducedCollection.h
  Object.h
  ObjectPool.h
  OutstandingCountTracker.cpp
  OutstandingCountTracker.h
  safe_dynamic_cast.h
  SharedPtrHash.h
  SharedPtrWrap.cpp
  SharedPtrWrap.h
  ThreadStatusMaintainer.cpp
  ThreadStatusMaintainer.h
)

if(BUILD_WINDOWS)
  set(Autowiring_SRCS
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
  CtorProxyTest.h
  CtorProxyTest.cpp
  CurrentContextPusherTest.h
  CurrentContextPusherTest.cpp
  DependentContextTest.h
  DependentContextTest.cpp
  EnclosedContextTestBase.h
  EnclosedContextTestBase.cpp
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
  LockReducedCollectionTest.h
  LockReducedCollectionTest.cpp
  MultiInheritTest.h
  MultiInheritTest.cpp
  PostConstructTest.h
  PostConstructTest.cpp
  ScopeTest.h
  ScopeTest.cpp
  SnoopTest.h
  SnoopTest.cpp
  TestFixtures/SimpleObject.h
  TestFixtures/SimpleThreaded.h
  TestFixtures/MultiInherit.h
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
  ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutowiringTest_SRCS)
  add_executable(AutowiringTest ${AutowiringTest_SRCS} "../gtest-all-guard.cpp")
  
  target_link_libraries(AutowiringTest Autowiring)
endif(BUILD_TESTING)