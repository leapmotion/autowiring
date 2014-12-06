if(NOT Boost_FOUND)
  find_package(Boost COMPONENTS system QUIET)
  if(NOT Boost_FOUND)
    message("Cannot build Autonet, boost not installed on this system")
    return()
  endif()
endif()

option(AUTOWIRING_BUILD_AUTONET "Build Autonet debugging server" ${AUTOWIRING_BUILD_AUTONET_DEFAULT})
if(NOT AUTOWIRING_BUILD_AUTONET)
  return()
endif()

if(autowiring_BUILD_ARM)
  message(FATAL_ERROR "Cannot currently build Autonet for ARM processors")
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

# All include files are located in /autowiring from here, so prepend that to all sources
rewrite_header_paths(AutoNet_SRCS)
add_pch(AutoNet_SRCS "stdafx.h" "stdafx.cpp")

add_library(AutoNet SHARED ${AutoNet_SRCS})
target_link_libraries(AutoNet PRIVATE ${Boost_LIBRARIES} Autowiring)
set_target_properties(AutoNet PROPERTIES INTERFACE_LINK_LIBRARIES Autowiring)

set_property(TARGET AutoNet PROPERTY FOLDER "Autowiring")

# On Mac, we have to figure out whether the target is 64-bit or fat
if(APPLE)
  foreach(i IN LISTS Boost_LIBRARIES)
    execute_process(COMMAND otool -arch all -vh "${i}" OUTPUT_VARIABLE LibProps)
    string(FIND ${LibProps} "x86_64" LibProps_MATCH_X64)
    string(FIND ${LibProps} "i386" LibProps_MATCH_I386)

    if(NOT LibProps_MATCH_X64 EQUAL -1)
      if(NOT LibProps_MATCH_I386 EQUAL -1)
        # Fat binary, no action necessary
      else()
        # 64-bit only, remove the 64-bit target
        message(WARNING "Boost library ${i} is not a fat binary, building Autonet for 64-bit only")
        set_target_properties(AutoNet PROPERTIES OSX_ARCHITECTURES "x86_64")
        set_target_properties(AutoNetTest PROPERTIES OSX_ARCHITECTURES "x86_64")
        set(NO_INSTALL_AUTONET ON)
      endif()
    else()
      message(${LibProps})
      message(FATAL_ERROR "Cannot build autonet with 32-bit boost binaries, set Boost_ROOT to point to a 64-bit boost installation")
    endif()
  endforeach()
endif()

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
