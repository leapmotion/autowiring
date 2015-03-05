set(AutowiringTest_SRCS
  AnySharedPointerTest.cpp
  ArgumentTypeTest.cpp
  AutoConfigTest.cpp
  AutoConstructTest.cpp
  AutoFilterCollapseRulesTest.cpp
  AutoFilterDiagnosticsTest.cpp
  AutoFilterFunctionTest.cpp
  AutoFilterSequencing.cpp
  AutoFilterTest.cpp
  AutoInjectableTest.cpp
  AutoPacketTest.cpp
  AutoPacketFactoryTest.cpp
  AutoParameterTest.cpp
  AutoRestarterTest.cpp
  AutowiringTest.cpp
  AutowiringUtilitiesTest.cpp
  BasicThreadTest.cpp
  BoltTest.cpp
  CoreContextTest.cpp
  CoreJobTest.cpp
  CoreRunnableTest.cpp
  CommonUseCasesTest.cpp
  ContextCleanupTest.cpp
  ContextEnumeratorTest.cpp
  ContextMapTest.cpp
  ContextMemberTest.cpp
  CoreThreadTest.cpp
  ContextCreatorTest.cpp
  CurrentContextPusherTest.cpp
  DecoratorTest.cpp
  DispatchQueueTest.cpp
  DtorCorrectnessTest.hpp
  DtorCorrectnessTest.cpp
  ExceptionFilterTest.cpp
  EventReceiverTest.cpp
  EventRegistryTest.cpp
  FactoryTest.cpp
  ForeignTest.cpp
  GlobalInitTest.hpp
  GlobalInitTest.cpp
  GuardObjectTest.cpp
  InterlockedRoutinesTest.cpp
  MultiInheritTest.cpp
  ObjectPoolTest.cpp
  PeerContextTest.cpp
  PostConstructTest.cpp
  SelfSelectingFixtureTest.cpp
  TeardownNotifierTest.cpp
  TypeRegistryTest.cpp
  SatisfiabilityTest.cpp
  ScopeTest.cpp
  SnoopTest.cpp
  TupleTest.cpp
  TestFixtures/custom_exception.hpp
  TestFixtures/ExitRaceThreaded.hpp
  TestFixtures/SimpleInterface.hpp
  TestFixtures/SimpleObject.hpp
  TestFixtures/SimpleThreaded.hpp
  TestFixtures/ThrowingListener.hpp
  TestFixtures/ThrowsWhenFired.hpp
  TestFixtures/ThrowsWhenRun.hpp
  TestFixtures/MultiInherit.hpp
  UuidTest.cpp
)

add_windows_sources(
  AutowiringTest_SRCS
  AutoFutureTest.cpp
)

set(AutowiringFixture_SRCS
  HasForwardOnlyType.hpp
  HasForwardOnlyType.cpp
  MentionsVariousOtherTypes.hpp
  MentionsVariousOtherTypes.cpp
  OtherSelectingFixture.hpp
  OtherSelectingFixture.cpp
  SelfSelectingFixture.hpp
  SelfSelectingFixture.cpp
)

add_pch(AutowiringFixture_SRCS "stdafx.h" "stdafx.cpp")
add_library(AutowiringFixture ${AutowiringFixture_SRCS})
target_include_directories(AutowiringFixture PRIVATE "${CMAKE_SOURCE_DIR}/contrib/autoboost")
set_property(TARGET AutowiringFixture PROPERTY FOLDER "Autowiring")

add_pch(AutowiringTest_SRCS "stdafx.h" "stdafx.cpp")
add_executable(AutowiringTest ${AutowiringTest_SRCS})
target_link_libraries(AutowiringTest Autowiring AutowiringFixture AutoTesting)
target_include_directories(AutowiringTest PRIVATE "${CMAKE_SOURCE_DIR}/contrib/autoboost")

# Need boost thread on android, because of our use of std::async
if(BUILD_ANDROID)
  find_package(Boost COMPONENTS atomic system thread REQUIRED QUIET)
  target_link_libraries(AutowiringTest ${Boost_LIBRARIES})
endif()

# Link AutoNet if we've got it
if(TARGET AutoNet)
  target_link_libraries(AutowiringTest AutoNet)
endif()

set_property(TARGET AutowiringTest PROPERTY FOLDER "Autowiring")

# This is a unit test, let CMake know this
add_test(NAME AutowiringTest COMMAND $<TARGET_FILE:AutowiringTest>)
