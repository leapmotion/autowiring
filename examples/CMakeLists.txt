cmake_minimum_required(VERSION 3.0.0)
include(../version.cmake)
find_package(autowiring ${autowiring_VERSION} REQUIRED)

add_executable(ContextExample ContextExample.cpp)
target_link_libraries(ContextExample Autowiring)
set_property(TARGET ContextExample PROPERTY FOLDER "Examples")

add_executable(ThreadExample ThreadExample.cpp)
target_link_libraries(ThreadExample Autowiring)
set_property(TARGET ThreadExample PROPERTY FOLDER "Examples")

add_executable(EventExample EventExample.cpp)
target_link_libraries(EventExample Autowiring)
set_property(TARGET EventExample PROPERTY FOLDER "Examples")

add_executable(AutoFilterExample AutoFilterExample.cpp)
target_link_libraries(AutoFilterExample Autowiring)
set_property(TARGET AutoFilterExample PROPERTY FOLDER "Examples")

if(TARGET AutoNet)
  find_package(Boost QUIET)
  if(NOT Boost_FOUND)
    message("Cannot build AutoNetExample, boost not installed on this system")
    return()
  endif()

  add_executable(AutoNetExample AutoNetExample.cpp)
  target_link_libraries(AutoNetExample Autowiring AutoNet)
  target_include_directories(AutoNetExample PRIVATE ${Boost_INCLUDE_DIR})
  set_property(TARGET AutoNetExample PROPERTY FOLDER "Examples")
endif()