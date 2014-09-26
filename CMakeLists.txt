cmake_minimum_required(VERSION 3.0)
include(version.cmake)
project(autowiring VERSION ${autowiring_VERSION})
include(CTest)

# Determine whether Autowiring has been embedded in another project
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(AUTOWIRING_IS_EMBEDDED)
else()
  set(AUTOWIRING_IS_EMBEDDED "TRUE")
endif()

# If there's an external libraries directory path, use it as a search prefix
if(EXTERNAL_LIBRARY_DIR)
  list(APPEND CMAKE_PREFIX_PATH ${EXTERNAL_LIBRARY_DIR})
  list(APPEND CMAKE_INCLUDE_PATH ${EXTERNAL_LIBRARY_DIR})
endif()

# Offer option for USE_LIBCXX only if not defined by enclosing project
# Check for existing definition of USE_LIBCXX
if(NOT DEFINED USE_LIBCXX)
  option(USE_LIBCXX "Build Autowiring using c++11" ON)
else()
  if(NOT USE_LIBCXX)
    message("Parent project has set USE_LIBCXX = OFF -> Build Autowiring using c++98")
  endif()
endif()

if(USE_LIBCXX)
  # Clang needs special additional flags to build with C++11
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    # Apple needs us to tell it that we're using libc++, or it will try to use libstdc++ instead
    message("AppleClang C++11")
    set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    message("Clang C++11")
    set(CMAKE_CXX_FLAGS "-std=c++11")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lstdc++")
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message("GCC C++11")
    set(CMAKE_CXX_FLAGS "-std=c++11")
  endif()
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
include(CMakeModules/pch.cmake)
set(AUTOWIRING_BUILD_AUTONET_DEFAULT ON)

# We don't build tests unless we're being built by ourselves in our own source tree
# When we're embedded in someone else's subtree, it's not likely that they will want
# us to bring all of our unit tests in with us.  Though, if they do desire this, they
# can always set the AUTOWIRING_BUILD_TESTS flag
get_filename_component(AUTOWIRING_ROOT_DIR . ABSOLUTE)
if(AUTOWIRING_IS_EMBEDDED)
  set(AUTOWIRING_BUILD_TESTS_DEFAULT OFF)
  set(AUTOWIRING_BUILD_EXAMPLES_DEFAULT OFF)
else()
  set(AUTOWIRING_BUILD_TESTS_DEFAULT ON)
  set(AUTOWIRING_BUILD_EXAMPLES_DEFAULT ON)
  
  # All of our binaries go to one place:  The binaries output directory.  We only want to tinker
  # with this if we're building by ourselves, otherwise we just do whatever the enclosing project
  # wants us to do.
  set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
endif()

option(AUTOWIRING_BUILD_TESTS "Build Autowiring unit tests" ${AUTOWIRING_BUILD_TESTS_DEFAULT})
function(add_googletest dirname)
  if(AUTOWIRING_BUILD_TESTS)
    add_subdirectory(${dirname})
  endif()
endfunction()

function(rewrite_header_paths target_SRC_VAR)
  # All include files are located in /autowiring from here, so prepend that to all sources
  set(SRCS_TEMP ${${target_SRC_VAR}})
  set(SRCS)
  foreach(i ${SRCS_TEMP})
    string(REGEX MATCH ".*\\.h$" hfile ${i})
    if(hfile)
      list(APPEND SRCS ${PROJECT_SOURCE_DIR}/autowiring/${i})
    else()
      list(APPEND SRCS ${i})
    endif()
  endforeach()
  set(${target_SRC_VAR} ${SRCS} PARENT_SCOPE)
endfunction()

# We have unit test projects via googletest, they're added in the places where they are defined
enable_testing()

# We want everyone to be compilable as though they have this root project in their search tree,
# because external entities will likely have just that
include_directories(
  .
  contrib
  contrib/websocketpp
)
add_subdirectory(src)
add_subdirectory(contrib)

# CMake configurations
configure_file(autowiring-config.cmake.in autowiring-config.cmake @ONLY)
configure_file(autowiring-configVersion.cmake.in autowiring-configVersion.cmake @ONLY)
configure_file(autowiring-config.h.in ${PROJECT_SOURCE_DIR}/autowiring/AutowiringConfig.h @ONLY)

# Only attempt to do anything with cpack if we're being built stand-alone
if(NOT AUTOWIRING_IS_EMBEDDED)
  # Install autowiring-config.cmake and autowiring-configVersion.cmake
  install (FILES
    "${CMAKE_CURRENT_BINARY_DIR}/autowiring-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/autowiring-configVersion.cmake"
    DESTINATION "cmake"
    COMPONENT autowiring
  )
  install(EXPORT AutowiringTargets FILE AutowiringTargets.cmake NAMESPACE Autowiring:: DESTINATION cmake)
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

  # Packaging stuff, if an installer is being made instead
  INCLUDE(InstallRequiredSystemLibraries)
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
endif()