cmake_minimum_required(VERSION 3.0.0)
include(../version.cmake)

project(examples)

set(autowiring_VERSION_REQUESTED ${autowiring_VERSION})
set(autowiring_VERSION)
set(autowiring_DEBUG ON)
find_package(autowiring ${autowiring_VERSION_REQUESTED} EXACT REQUIRED NO_CMAKE_BUILDS_PATH)
message("Found autowiring ${autowiring_VERSION}")
message("Include dir is ${autowiring_INCLUDE_DIR}")

# Clang needs special additional flags to build with C++11
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
  # Apple needs us to tell it that we're using libc++, or it will try to use libstdc++ instead
  message("AppleClang C++11")
  set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  message("Clang C++11")
  set(CMAKE_CXX_FLAGS "-std=c++11")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lstdc++")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  message("GCC C++11")
  set(CMAKE_CXX_FLAGS "-std=c++11")
endif()


add_executable(ContextExample ContextExample.cpp)
target_link_libraries(ContextExample Autowiring::Autowiring)

add_executable(ThreadExample ThreadExample.cpp)
target_link_libraries(ThreadExample Autowiring::Autowiring)

add_executable(AutoFilterExample AutoFilterExample.cpp)
target_link_libraries(AutoFilterExample Autowiring::Autowiring)

add_executable(AutoFilterTutorial.0.0 AutoFilterTutorial.0.0.cpp)
target_link_libraries(AutoFilterTutorial.0.0 Autowiring::Autowiring)

add_executable(AutoFilterTutorial.1.0 AutoFilterTutorial.1.0.cpp)
target_link_libraries(AutoFilterTutorial.1.0 Autowiring::Autowiring)

# Need multithreading services if available
find_package(Threads)
if(Threads_FOUND)
  if(CMAKE_USE_PTHREADS_INIT)
    target_link_libraries(ContextExample ${CMAKE_THREAD_LIBS_INIT})
    target_link_libraries(ThreadExample ${CMAKE_THREAD_LIBS_INIT})
    target_link_libraries(EventExample ${CMAKE_THREAD_LIBS_INIT})
    target_link_libraries(AutoFilterExample ${CMAKE_THREAD_LIBS_INIT})
  endif()
endif()

if(TARGET Autowiring::AutoNet)
  add_executable(AutoNetExample AutoNetExample.cpp)
  target_link_libraries(AutoNetExample Autowiring::Autowiring Autowiring::AutoNet)
  if(Threads_FOUND)
    if(CMAKE_USE_PTHREADS_INIT)
      target_link_libraries(AutoNetExample ${CMAKE_THREAD_LIBS_INIT})
    endif()
  endif()
endif()
