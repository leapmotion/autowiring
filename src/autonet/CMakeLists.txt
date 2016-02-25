include_directories(
  ${PROJECT_SOURCE_DIR}/contrib/autoboost
)

set(AutoNet_SRCS
  AutoNetServer.cpp
  AutoNetServer.h
  AutoNetServerImpl.cpp
  AutoNetServerImpl.hpp
  AutoNetTransportHttp.hpp
  AutoNetTransportHttp.cpp
  ${PROJECT_SOURCE_DIR}/contrib/json11/json11.cpp
  ${PROJECT_SOURCE_DIR}/contrib/json11/json11.hpp
)

# All include files are located in /autowiring from here, so prepend that to all sources
add_pch(AutoNet_SRCS "stdafx.h" "stdafx.cpp")

add_library(AutoNet STATIC ${AutoNet_SRCS})
target_link_libraries(AutoNet PRIVATE Autowiring)
set_target_properties(AutoNet PROPERTIES INTERFACE_LINK_LIBRARIES Autowiring)

add_subdirectory(test)

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
