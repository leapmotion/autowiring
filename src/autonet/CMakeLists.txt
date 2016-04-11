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

add_library(AutoNet STATIC ${AutoNet_SRCS})
add_pch(AutoNet "stdafx.h" "stdafx.cpp")
target_link_libraries(AutoNet PRIVATE Autowiring)
set_target_properties(AutoNet PROPERTIES INTERFACE_LINK_LIBRARIES Autowiring)

add_subdirectory(test)

#
# Install library
#

# Install public header files
install_headers(TARGET AutoNet DESTINATION include/autonet COMPONENT autowiring)

if(NOT NO_INSTALL_AUTONET AND NOT AUTOWIRING_IS_EMBEDDED)
  install(TARGETS AutoNet EXPORT AutowiringTargets
    DESTINATION lib
    COMPONENT autowiring
    CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
  )
endif()
