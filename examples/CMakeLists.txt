cmake_minimum_required(VERSION 3.0.0)
include(../version.cmake)
find_package(autowiring ${autowiring_VERSION} REQUIRED)

# Use C++11
if(USE_LIBCXX)
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
endif()


add_executable(ContextExample ContextExample.cpp)
target_link_libraries(ContextExample Autowiring)

add_executable(ThreadExample ThreadExample.cpp)
target_link_libraries(ThreadExample Autowiring)

add_executable(EventExample EventExample.cpp)
target_link_libraries(EventExample Autowiring)

add_executable(AutoFilterExample AutoFilterExample.cpp)
target_link_libraries(AutoFilterExample Autowiring)

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
  find_package(Boost COMPONENTS thread atomic chrono system date_time regex QUIET)
  if(NOT Boost_FOUND)
    message("Cannot build AutoNetExample, boost not installed on this system")
    return()
  endif()

  include_directories(
    ${Boost_INCLUDE_DIR}
  )

  add_executable(AutoNetExample AutoNetExample.cpp)
  target_link_libraries(AutoNetExample Autowiring AutoNet ${Boost_LIBRARIES})
  if(Threads_FOUND)
    if(CMAKE_USE_PTHREADS_INIT)
      target_link_libraries(AutoNetExample ${CMAKE_THREAD_LIBS_INIT})
    endif()
  endif()
endif()
