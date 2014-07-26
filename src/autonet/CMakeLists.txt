find_package(websocketpp QUIET)
if(NOT websocketpp_FOUND)
  message("Skipping Autonet build, websocketpp not found")
  return()
endif()

find_package(Boost QUIET)
if(NOT Boost_FOUND)
  message("Cannot build Autonet, boost not installed on this system")
  return()
endif()

add_subdirectory(test)
include_directories(${Boost_INCLUDE_DIR} ${WEBSOCKETPP_INCLUDE_DIR})

set(AutoNet_SRCS
  AutoNetServer.h
  AutoNetServer.cpp
  Jzon.h
  Jzon.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutoNet_SRCS)
add_library(AutoNet STATIC ${AutoNet_SRCS})

add_definitions(-USEAUTONET)
