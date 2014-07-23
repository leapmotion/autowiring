include_directories(
    ../src
    ..
    gtest-1.7.0/fused-src
    gtest-1.7.0/include
  )

OPTION(BUILD_AUTONET "Enable autonet features of autowiring project." OFF)

set(AutowiringTest_SRCS
  AnySharedPointerTest.h
  AnySharedPointerTest.cpp
  AutoAnchorTest.h
  AutoAnchorTest.cpp
  AutoFilterTest.h
  AutoFilterTest.cpp
  AutoInjectableTest.h
  AutoInjectableTest.cpp
  AutoPacketFactoryTest.h
  AutoPacketFactoryTest.cpp
  AutowiringBenchmarkTest.h
  AutowiringBenchmarkTest.cpp
  AutowiringTest.h
  AutowiringTest.cpp
  AutowiringUtilitiesTest.h
  AutowiringUtilitiesTest.cpp
  BasicThreadTest.h
  BasicThreadTest.cpp
  CoreContextTest.h
  CoreContextTest.cpp
  ContextCleanupTest.h
  ContextCleanupTest.cpp
  ContextEnumeratorTest.h
  ContextEnumeratorTest.cpp
  ContextMapTest.h
  ContextMapTest.cpp
  ContextMemberTest.h
  ContextMemberTest.cpp
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
  GuardObjectTest.h
  GuardObjectTest.cpp
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
  gtest-all-guard.cpp
)

if(BUILD_AUTONET)
  set(
    AutowiringTest_SRCS
    AutoNetServerTest.h
    AutoNetServerTest.cpp
    ${AutowiringTest_SRCS}
  )
endif()

set(
  AutowiringFixture_SRCS
  SelfSelectingFixture.h
  SelfSelectingFixture.cpp
  OtherSelectingFixture.h
  OtherSelectingFixture.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutowiringFixture_SRCS)
add_library(AutowiringFixture ${AutowiringFixture_SRCS})
set_property(TARGET AutowiringFixture PROPERTY FOLDER "Autowiring")

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutowiringTest_SRCS)
add_executable(AutowiringTest ${AutowiringTest_SRCS})
target_link_libraries(AutowiringTest Autowiring AutowiringFixture)
set_property(TARGET AutowiringTest PROPERTY FOLDER "Autowiring")
