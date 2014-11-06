find_package(Boost COMPONENTS system QUIET)
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

# Needed to link boost on windows
if(WIN32)
  link_directories(${Boost_LIBRARY_DIRS})
endif()

set(AutoNet_SRCS
  AutoNetServer.cpp
  AutoNetServer.h
  AutoNetServerImpl.cpp
  AutoNetServerImpl.hpp
  ${PROJECT_SOURCE_DIR}/contrib/json11/json11.cpp
  ${PROJECT_SOURCE_DIR}/contrib/json11/json11.hpp
)

set(AutoBoostWebSocketServer_SRCS
  AutoBoostWebSocketServer.cpp
  AutoBoostWebSocketServer.hpp
  IWebSocketServer.hpp
  ${PROJECT_SOURCE_DIR}/contrib/json11/json11.cpp
  ${PROJECT_SOURCE_DIR}/contrib/json11/json11.hpp
)

# All include files are located in /autowiring from here, so prepend that to all sources
rewrite_header_paths(AutoBoostWebSocketServer_SRCS)
ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutoBoostWebSocketServer_SRCS)
add_library(abwss SHARED ${AutoBoostWebSocketServer_SRCS})
target_link_libraries(abwss ${Boost_LIBRARIES})
set_property(TARGET abwss PROPERTY FOLDER "Autowiring")

# All include files are located in /autowiring from here, so prepend that to all sources
rewrite_header_paths(AutoNet_SRCS)
ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutoNet_SRCS)
add_library(AutoNet ${AutoNet_SRCS})
set_target_properties(AutoNet PROPERTIES INTERFACE_LINK_LIBRARIES "Autowiring;abwss")

set_property(TARGET AutoNet PROPERTY FOLDER "Autowiring")

#
# Install library
#

if(NOT AUTOWIRING_IS_EMBEDDED)
  install(TARGETS AutoNet abwss EXPORT AutowiringTargets
    DESTINATION lib
    COMPONENT autowiring 
    CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
  )
endif()
