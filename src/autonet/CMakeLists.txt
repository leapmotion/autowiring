include_directories(..)

find_package(websocketpp)
if(NOT websocketpp_FOUND)
  message("Skipping Autonet build, websocketpp not found")
  return()
endif()

find_package(Boost)
if(NOT Boost_FOUND)
  message("Cannot build Autonet, boost not installed on this system")
  return()
endif()

include_directories(${Boost_INCLUDE_DIR})

set(AutoNet_SRCS
  AutoNetServer.h
  AutoNetServer.cpp
  Jzon.h
  Jzon.cpp
)

include_directories(${WEBSOCKETPP_INCLUDE_DIR})
ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutoNet_SRCS)
add_library(AutoNet STATIC ${AutoNet_SRCS})

add_definitions(-USEAUTONET)
