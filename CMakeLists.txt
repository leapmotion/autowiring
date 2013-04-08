include_directories(
    ../googletest
    ../googletest/include
    ..
  )

set(
  Autowiring_SRCS
  Autowired.cpp
  Autowired.h
  Autowirer.cpp
  Autowirer.h
  CMakeLists.txt
  ContextMember.cpp
  ContextMember.h
  CoreContext.cpp
  CoreContext.h
  CoreThread.cpp
  CoreThread.h
  GlobalCoreContext.cpp
  GlobalCoreContext.h
  Object.h
  SharedPtrWrap.cpp
  SharedPtrWrap.h
)

set(
  AutowiringTest_SRCS
  TestMain.cpp
  ContextCleanupTest.h
  ContextCleanupTest.cpp
  GlobalInitTest.h
  GlobalInitTest.cpp
  MultiInheritTest.h
  MultiInheritTest.cpp
  ScopeTest.h
  ScopeTest.cpp
  TestFixtures/SimpleObject.h
  TestFixtures/MultiInherit.h
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" Autowiring_SRCS)
add_library(Autowiring STATIC ${Autowiring_SRCS})

if(BUILD_TESTING)
  ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutowiringTest_SRCS)
  add_executable(AutowiringTest ${AutowiringTest_SRCS} "../googletest/src/gtest-all.cc")
  target_link_libraries(AutowiringTest Autowiring ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY})
endif(BUILD_TESTING)