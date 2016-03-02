set(AutowiringTest_SRCS
  AnySharedPointerTest.cpp
  ArgumentTypeTest.cpp
  AtomicListTest.cpp
  AutoConfigTest.cpp
  AutoConstructTest.cpp
  AutoFilterAltitudeTest.cpp
  AutoFilterCollapseRulesTest.cpp
  AutoFilterConstructRulesTest.cpp
  AutoFilterDescriptorTest.cpp
  AutoFilterDiagnosticsTest.cpp
  AutoFilterFunctionTest.cpp
  AutoFilterMultiDecorateTest.cpp
  AutoFilterRvalueTest.cpp
  AutoFilterSequencing.cpp
  AutoFilterTest.cpp
  AutoIDTest.cpp
  AutoPacketTest.cpp
  AutoPacketFactoryTest.cpp
  AutoSignalTest.cpp
  AutowiringDebugTest.cpp
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
  CreationRulesTest.cpp
  CurrentContextPusherTest.cpp
  DecoratorTest.cpp
  DemangleTest.cpp
  DispatchQueueTest.cpp
  DtorCorrectnessTest.cpp
  ExceptionFilterTest.cpp
  FactoryTest.cpp
  GlobalInitTest.hpp
  GlobalInitTest.cpp
  HeteroBlockTest.cpp
  InterlockedRoutinesTest.cpp
  MultiInheritTest.cpp
  ObjectPoolTest.cpp
  ObservableTest.cpp
  OnceTest.cpp
  ParallelTest.cpp
  PostConstructTest.cpp
  SelfSelectingFixtureTest.cpp
  SpinLockTest.cpp
  TeardownNotifierTest.cpp
  TypeRegistryTest.cpp
  SatisfiabilityTest.cpp
  ScopeTest.cpp
  SnoopTest.cpp
  ThreadPoolTest.cpp
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

add_pch(AutowiringTest_SRCS "stdafx.h" "stdafx.cpp")
add_executable(AutowiringTest ${AutowiringTest_SRCS})
target_link_libraries(AutowiringTest Autowiring AutowiringFixture AutoTesting)
target_include_directories(AutowiringTest PRIVATE "${CMAKE_SOURCE_DIR}/contrib/autoboost")

# Link AutoNet if we've got it
if(TARGET AutoNet)
  target_link_libraries(AutowiringTest AutoNet)
endif()

# This is a unit test, let CMake know this
add_test(NAME AutowiringTest COMMAND $<TARGET_FILE:AutowiringTest>)
