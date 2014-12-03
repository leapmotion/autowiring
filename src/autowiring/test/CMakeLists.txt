set(AutowiringTest_SRCS
  AnySharedPointerTest.cpp
  ArgumentTypeTest.cpp
  AutoConfigTest.cpp
  AutoConstructTest.cpp
  AutoFilterCollapseRulesTest.cpp
  AutoInjectableTest.cpp
  AutoPacketFactoryTest.cpp
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
  MarshalingTest.cpp
  MultiInheritTest.cpp
  ObjectPoolTest.cpp
  PeerContextTest.cpp
  PostConstructTest.cpp
  SelfSelectingFixtureTest.cpp
  TeardownNotifierTest.cpp
  TypeRegistryTest.cpp
  ScopeTest.cpp
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
  UuidTest.cpp
)

if(autowiring_USE_LIBCXX)
  set(AutowiringTest_SRCS ${AutowiringTest_SRCS}
    AutoFilterFunctionTest.cpp
    AutoFilterSequencing.cpp
    AutoFilterTest.cpp
  )
endif()

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

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutowiringFixture_SRCS)
add_library(AutowiringFixture ${AutowiringFixture_SRCS})
set_property(TARGET AutowiringFixture PROPERTY FOLDER "Autowiring")
if(NOT WIN32)
  set_target_properties(AutowiringFixture PROPERTIES COMPILE_FLAGS "-fvisibility=hidden")
endif()

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutowiringTest_SRCS)
add_executable(AutowiringTest ${AutowiringTest_SRCS})
target_link_libraries(AutowiringTest Autowiring AutowiringFixture AutoTesting)
if(NOT WIN32 AND NOT autowiring_USE_LIBCXX)
  find_package(Boost COMPONENTS thread atomic chrono system date_time QUIET)
  target_link_libraries(AutowiringTest ${Boost_LIBRARIES})
endif()

# Need boost thread on android, because of our use of std::async
if(BUILD_ANDROID)
  target_link_libraries(AutowiringTest ${Boost_LIBRARIES})
endif()

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
