if(NOT WIN32 AND NOT autowiring_USE_LIBCXX)
  message("Cannot build Autonet, requires proper C++11 supprt")
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
)

add_conditional_sources(
  AutoNet_SRCS
  "ON"
  GROUP_NAME "Autoboost"
  FILES
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/system/src/error_code.cpp
)

add_conditional_sources(
  AutoNet_SRCS
  "NOT MSVC"
  GROUP_NAME "Autoboost"
  FILES
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/thread/src/pthread/once_atomic.cpp
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/thread/src/pthread/thread.cpp
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/thread/src/future.cpp
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
