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

# We want everyone to be compilable as though they have this root project in their search tree,
# because external entities will likely have just that
include_directories(.)
add_subdirectory(src)

configure_file(autowiring-config.cmake.in autowiring-config.cmake @ONLY)
configure_file(autowiring-configVersion.cmake.in autowiring-configVersion.cmake @ONLY)

# We have unit test projects via googletest, they're added in the places where they are defined
enable_testing()

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
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Autowiring")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "${autowiring_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${autowiring_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${autowiring_VERSION_PATCH}")
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "autowiring")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "autowiring ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
INCLUDE(CPack)
