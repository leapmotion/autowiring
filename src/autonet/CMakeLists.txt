find_package(Boost COMPONENTS regex system QUIET)
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
  ${PROJECT_SOURCE_DIR}/contrib/json11/json11.cpp
  ${PROJECT_SOURCE_DIR}/contrib/json11/json11.hpp
)

# All include files are located in /autowiring from here, so prepend that to all sources
rewrite_header_paths(AutoNet_SRCS)
ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutoNet_SRCS)

add_library(AutoNet STATIC ${AutoNet_SRCS})
set_target_properties(AutoNet PROPERTIES DEBUG_POSTFIX d)
target_link_libraries(AutoNet Autowiring)
set_property(TARGET AutoNet PROPERTY FOLDER "Autowiring")

#
# Install library
#

if(NOT AUTOWIRING_IS_EMBEDDED)
  install(TARGETS AutoNet EXPORT AutowiringTargets
    DESTINATION lib
    COMPONENT autowiring 
    CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
  )
endif()
