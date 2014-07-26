set(AutowiringTest_SRCS
  AnySharedPointerTest.hpp
  AnySharedPointerTest.cpp
  AutoAnchorTest.hpp
  AutoAnchorTest.cpp
  AutoFilterTest.hpp
  AutoFilterTest.cpp
  AutoInjectableTest.hpp
  AutoInjectableTest.cpp
  AutoPacketFactoryTest.hpp
  AutoPacketFactoryTest.cpp
  AutowiringBenchmarkTest.hpp
  AutowiringBenchmarkTest.cpp
  AutowiringTest.hpp
  AutowiringTest.cpp
  AutowiringUtilitiesTest.hpp
  AutowiringUtilitiesTest.cpp
  BasicThreadTest.hpp
  BasicThreadTest.cpp
  CoreContextTest.hpp
  CoreContextTest.cpp
  ContextCleanupTest.hpp
  ContextCleanupTest.cpp
  ContextEnumeratorTest.hpp
  ContextEnumeratorTest.cpp
  ContextMapTest.hpp
  ContextMapTest.cpp
  ContextMemberTest.hpp
  ContextMemberTest.cpp
  CoreThreadTest.hpp
  CoreThreadTest.cpp
  BoltTest.hpp
  BoltTest.cpp
  ContextCreatorTest.hpp
  ContextCreatorTest.cpp
  CurrentContextPusherTest.hpp
  CurrentContextPusherTest.cpp
  DecoratorTest.hpp
  DecoratorTest.cpp
  DispatchQueueTest.hpp
  DispatchQueueTest.cpp
  DtorCorrectnessTest.hpp
  DtorCorrectnessTest.cpp
  EnclosedContextTestBase.hpp
  ExceptionFilterTest.hpp
  ExceptionFilterTest.cpp
  EventReceiverTest.hpp
  EventReceiverTest.cpp
  FactoryTest.hpp
  FactoryTest.cpp
  GlobalInitTest.hpp
  GlobalInitTest.cpp
  GuardObjectTest.hpp
  GuardObjectTest.cpp
  InterlockedRoutinesTest.hpp
  InterlockedRoutinesTest.cpp
  MarshalingTest.hpp
  MarshalingTest.cpp
  MultiInheritTest.hpp
  MultiInheritTest.cpp
  ObjectPoolTest.hpp
  ObjectPoolTest.cpp
  PeerContextTest.hpp
  PeerContextTest.cpp
  PostConstructTest.hpp
  PostConstructTest.cpp
  SelfSelectingFixtureTest.hpp
  SelfSelectingFixtureTest.cpp
  TeardownNotifierTest.hpp
  TeardownNotifierTest.cpp
  CoreJobTest.hpp
  CoreJobTest.cpp
  TypeRegistryTest.hpp
  TypeRegistryTest.cpp
  ScopeTest.hpp
  ScopeTest.cpp
  SnoopTest.hpp
  SnoopTest.cpp
  TestFixtures/custom_exception.hpp
  TestFixtures/ExitRaceThreaded.hpp
  TestFixtures/SimpleInterface.hpp
  TestFixtures/SimpleObject.hpp
  TestFixtures/SimpleThreaded.hpp
  TestFixtures/ThrowingListener.hpp
  TestFixtures/ThrowsWhenFired.hpp
  TestFixtures/ThrowsWhenRun.hpp
  TestFixtures/MultiInherit.hpp
  UuidTest.hpp
  UuidTest.cpp
  gtest-all-guard.cpp
)

if(BUILD_AUTONET)
  set(
    AutowiringTest_SRCS
    AutoNetServerTest.hpp
    AutoNetServerTest.cpp
    ${AutowiringTest_SRCS}
  )
endif()

set(
  AutowiringFixture_SRCS
  SelfSelectingFixture.hpp
  SelfSelectingFixture.cpp
  OtherSelectingFixture.hpp
  OtherSelectingFixture.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutowiringFixture_SRCS)
add_library(AutowiringFixture ${AutowiringFixture_SRCS})
set_property(TARGET AutowiringFixture PROPERTY FOLDER "Autowiring")

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutowiringTest_SRCS)
add_executable(AutowiringTest ${AutowiringTest_SRCS})
target_link_libraries(AutowiringTest Autowiring AutowiringFixture)
set_property(TARGET AutowiringTest PROPERTY FOLDER "Autowiring")

# This is a unit test, let CMake know this
add_test(NAME AutowiringTest COMMAND $<TARGET_FILE:AutowiringTest>)
