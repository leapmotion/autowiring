find_package(Boost COMPONENTS regex QUIET)
if(NOT Boost_FOUND)
  message("Cannot build Autonet, boost not installed on this system")
  return()
endif()

option(AUTOWIRING_BUILD_AUTONET "Build Autonet debugging server" ${AUTOWIRING_BUILD_AUTONET_DEFAULT})
if(NOT AUTOWIRING_BUILD_AUTONET)
  return()
endif()

add_googletest(test)
include_directories(
  ${Boost_INCLUDE_DIR}
)

set(AutoNet_SRCS
  AutoNetServer.cpp
  AutoNetServer.h
  AutoNetServerImpl.cpp
  AutoNetServerImpl.hpp
)

# All include files are located in /autowiring from here, so prepend that to all sources
set(AutoNet_SRCS_TEMP ${AutoNet_SRCS})
set(AutoNet_SRCS)
foreach(i ${AutoNet_SRCS_TEMP})
  string(REGEX MATCH ".*\\.h$" hfile ${i})
  if(hfile)
    list(APPEND AutoNet_SRCS ${PROJECT_SOURCE_DIR}/autowiring/${i})
  else()
    list(APPEND AutoNet_SRCS ${i})
  endif()
endforeach()

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutoNet_SRCS)
add_library(AutoNet STATIC ${AutoNet_SRCS})
target_link_libraries(AutoNet Autowiring json11 ${Boost_LIBRARIES})
set_property(TARGET AutoNet PROPERTY FOLDER "Autowiring")
