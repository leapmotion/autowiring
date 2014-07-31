cmake_minimum_required(VERSION 2.8)
project(autowiring)
include(CTest)

# TODO: Use the VERSION attribute for the "project" setting instead after upgrading
# the cmake_minimum_required to version 3.0
set(autowiring_VERSION_MAJOR 0)
set(autowiring_VERSION_MINOR 1)
set(autowiring_VERSION_PATCH 0)

# Clang needs special additional flags to build with C++11
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  message("Clang C++11")
  set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  message("GCC C++11")
  set(CMAKE_CXX_FLAGS "-std=c++11")
endif()

include(CMakeModules/pch.cmake)

# We don't build tests unless we're being built by ourselves in our own source tree
# When we're embedded in someone else's subtree, it's not likely that they will want
# us to bring all of our unit tests in with us.  Though, if they do desire this, they
# can always set the AUTOWIRING_BUILD_TESTS flag
get_filename_component(AUTOWIRING_ROOT_DIR . ABSOLUTE)
if(CMAKE_SOURCE_DIR STREQUAL AUTOWIRING_ROOT_DIR)
  set(AUTOWIRING_BUILD_TESTS_DEFAULT ON)
  set(AUTOWIRING_BUILD_EXAMPLES_DEFAULT ON)
  set(AUTOWIRING_BUILD_AUTONET_DEFAULT ON)
  
  # All of our binaries go to one place:  The binaries output directory.  We only want to tinker
  # with this if we're building by ourselves, otherwise we just do whatever the enclosing project
  # wants us to do.
  set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
else()
  set(AUTOWIRING_BUILD_TESTS_DEFAULT OFF)
  set(AUTOWIRING_BUILD_EXAMPLES_DEFAULT OFF)
  set(AUTOWIRING_BUILD_AUTONET_DEFAULT OFF)
endif()

option(AUTOWIRING_BUILD_TESTS "Build Autowiring unit tests" ${AUTOWIRING_BUILD_TESTS_DEFAULT})
function(add_googletest dirname)
  if(AUTOWIRING_BUILD_TESTS)
    add_subdirectory(${dirname})
  endif()
endfunction()

# We have unit test projects via googletest, they're added in the places where they are defined
enable_testing()

# We want everyone to be compilable as though they have this root project in their search tree,
# because external entities will likely have just that
include_directories(
  .
  contrib
)
add_subdirectory(src)
add_subdirectory(contrib)
# Build examples
option(AUTOWIRING_BUILD_EXAMPLES "Build Autowiring examples" ${AUTOWIRING_BUILD_EXAMPLES_DEFAULT})
if(AUTOWIRING_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

# CMake configurations
configure_file(autowiring-config.cmake.in autowiring-config.cmake @ONLY)
configure_file(autowiring-configVersion.cmake.in autowiring-configVersion.cmake @ONLY)

# Install autowiring-config.cmake and autowiring-configVersion.cmake
install (FILES
  "autowiring-config.cmake"
  "autowiring-configVersion.cmake"
  DESTINATION cmake
  COMPONENT autowiring
)

# This is the upgrade GUID.  Part of the GUID is derived from the major version number.  Any time
# the major version number is adjusted, the upgrade GUID changes.  This allows multiple versions
# of the same product to be installed on a user's system at the same time, but also means that
# manual uninstallation of older versions is required.
#
# For more information on the rationale for this process, see the discussion on semantic versioning
# found at http://semver.org/
SET(CPACK_WIX_UPGRADE_GUID "{060E5EDD-229${autowiring_VERSION_MAJOR}-4AD8-BAFA-A303D5696A2D}")

# Need a custom wix installation template so that we update the CMake package registry correctly
# Only really needed on Windows; Mac and Linux have pretty good default search behavior, so we
# leave those alone.
SET(CPACK_WIX_TEMPLATE autowiring.wxs)

# Packaging stuff, if an installer is being made insteadINCLUDE(InstallRequiredSystemLibraries)
SET(CPACK_GENERATOR "WIX")
SET(CPACK_PACKAGE_VENDOR "Leap Motion")
SET(CPACK_PACKAGE_CONTACT "cmercenary@gmail.com")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Autowiring")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "${autowiring_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${autowiring_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${autowiring_VERSION_PATCH}")
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "autowiring")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "autowiring ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
INCLUDE(CPack)
