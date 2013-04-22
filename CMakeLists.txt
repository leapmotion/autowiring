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
  ContextMap.h
  ContextMap.cpp
  ContextMember.cpp
  ContextMember.h
  CoreContext.cpp
  CoreContext.h
  CoreThread.cpp
  CoreThread.h
  CurrentContextPusher.cpp
  CurrentContextPusher.h
  EventManager.h
  InstantiatorLink.h  
  GlobalCoreContext.cpp
  GlobalCoreContext.h
  Object.h
  SharedPtrWrap.cpp
  SharedPtrWrap.h
  ThreadStatusMaintainer.cpp
  ThreadStatusMaintainer.h
)

set(
  AutowiringTest_SRCS
  TestMain.cpp
  ContextCleanupTest.h
  ContextCleanupTest.cpp
  ContextMapTest.h
  ContextMapTest.cpp
  CurrentContextPusherTest.h
  CurrentContextPusherTest.cpp
  DependentContextTest.h
  DependentContextTest.cpp
  GlobalInitTest.h
  GlobalInitTest.cpp
  MultiInheritTest.h
  MultiInheritTest.cpp
  ScopeTest.h
  ScopeTest.cpp
  TestFixtures/SimpleObject.h
  TestFixtures/SimpleThreaded.h
  TestFixtures/MultiInherit.h
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" Autowiring_SRCS)
add_library(Autowiring STATIC ${Autowiring_SRCS})

if(BUILD_TESTING)
  ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutowiringTest_SRCS)
  add_executable(AutowiringTest ${AutowiringTest_SRCS} "../googletest/src/gtest-all.cc")
  if(NOT BUILD_WINDOWS)
    set(Boost_SYSTEM_LIBRARY libboost_system)
    add_library(${Boost_SYSTEM_LIBRARY} STATIC IMPORTED)
    set_property(TARGET ${Boost_SYSTEM_LIBRARY} PROPERTY IMPORTED_LOCATION ${Boost_LIBRARY_DIRS}/${Boost_SYSTEM_LIBRARY}${STATIC_LIBRARY_SUFFIX})
	
    set(Boost_THREAD_LIBRARY libboost_thread)
    add_library(${Boost_THREAD_LIBRARY} STATIC IMPORTED)
    set_property(TARGET ${Boost_THREAD_LIBRARY} PROPERTY IMPORTED_LOCATION ${Boost_LIBRARY_DIRS}/${Boost_THREAD_LIBRARY}${STATIC_LIBRARY_SUFFIX})
  endif()
  
  target_link_libraries(AutowiringTest Autowiring ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY})
endif(BUILD_TESTING)