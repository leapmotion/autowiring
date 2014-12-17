option(AUTOWIRING_BUILD_AUTONET "Build Autonet debugging server" ${AUTOWIRING_BUILD_AUTONET_DEFAULT})
if(NOT AUTOWIRING_BUILD_AUTONET)
  return()
endif()

if(NOT WIN32 AND NOT autowiring_USE_LIBCXX)
  message("Cannot build Autonet, requires proper C++11 supprt")
endif()

if(autowiring_BUILD_ARM)
  message(FATAL_ERROR "Cannot currently build Autonet for ARM processors")
endif()

add_googletest(test)
include_directories(
  ${PROJECT_SOURCE_DIR}/contrib/autoboost
)

set(AutoNet_SRCS
  AutoNetServer.cpp
  AutoNetServer.h
  AutoNetServerImpl.cpp
  AutoNetServerImpl.hpp
  ${PROJECT_SOURCE_DIR}/contrib/json11/json11.cpp
  ${PROJECT_SOURCE_DIR}/contrib/json11/json11.hpp
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/system/src/error_code.cpp
)

# All include files are located in /autowiring from here, so prepend that to all sources
rewrite_header_paths(AutoNet_SRCS)
add_pch(AutoNet_SRCS "stdafx.h" "stdafx.cpp")

add_library(AutoNet STATIC ${AutoNet_SRCS})
target_link_libraries(AutoNet PRIVATE Autowiring)
set_target_properties(AutoNet PROPERTIES INTERFACE_LINK_LIBRARIES Autowiring)

set_property(TARGET AutoNet PROPERTY FOLDER "Autowiring")

#
# Install library
#

if(NOT NO_INSTALL_AUTONET AND NOT AUTOWIRING_IS_EMBEDDED)
  install(TARGETS AutoNet EXPORT AutowiringTargets
    DESTINATION lib
    COMPONENT autowiring
    CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
  )
endif()
