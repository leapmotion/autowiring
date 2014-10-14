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
else()
  set(AUTOWIRING_BUILD_TESTS_DEFAULT ON)
  
  # All of our binaries go to one place:  The binaries output directory.  We only want to tinker
  # with this if we're building by ourselves, otherwise we just do whatever the enclosing project
  # wants us to do.
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

  # Libraries not only all wind up in the libraries directory, but we also keep them all together
  # here by putting them in the same place, regardless of whether they are debug or release.  This
  # makes globbing them together much easier.
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
endif()

# Postfix on all debug libraries should be "d"
set(CMAKE_DEBUG_POSTFIX d)

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

# CMake configurations
configure_file(autowiring-config.cmake.in autowiring-config.cmake @ONLY)
configure_file(autowiring-configVersion.cmake.in autowiring-configVersion.cmake @ONLY)
configure_file(AutowiringConfig.h.in ${PROJECT_SOURCE_DIR}/autowiring/AutowiringConfig.h @ONLY)

# Recurse through source directories
add_subdirectory(src)

# Export library
export(EXPORT AutowiringTargets FILE AutowiringTargets.cmake NAMESPACE Autowiring::)

# Only attempt to do anything with cpack if we're being built stand-alone
if(NOT AUTOWIRING_IS_EMBEDDED)
  # Install autowiring-config.cmake and autowiring-configVersion.cmake
  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/autowiring-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/autowiring-configVersion.cmake"
    DESTINATION "cmake"
    COMPONENT autowiring
  )

  # Install public header files
  install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/autowiring/
    DESTINATION include/autowiring
    COMPONENT autowiring
    FILES_MATCHING PATTERN "*.h"
  )

  # Targets file is needed in order to describe how to link Autowiring to the rest of the system
  install(EXPORT AutowiringTargets FILE AutowiringTargets.cmake COMPONENT autowiring NAMESPACE Autowiring:: DESTINATION cmake CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES})

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
  SET(CPACK_MONOLITHIC_INSTALL ON)
  
  # Run the script that will grab the debug and release configurations and install them during packaging
  set(CPACK_INSTALL_COMMANDS
    "${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config Debug --target install"
    "${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config Release --target install"
  )

  # Pick the generator in an appropriate way
  if(WIN32)
    set(CPACK_GENERATOR WIX ZIP)
  elseif(APPLE)
    # TODO: Add Bundle as a generator here
    set(CPACK_GENERATOR TGZ)
  else()
    set(CPACK_GENERATOR TGZ DEB)
  endif()

  # Packaging stuff, if an installer is being made instead
  SET(CPACK_PACKAGE_VENDOR "Leap Motion")
  SET(CPACK_PACKAGE_CONTACT "cmercenary@gmail.com")
  SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
  SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Autowiring")
  SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
  SET(CPACK_PACKAGE_VERSION_MAJOR "${autowiring_VERSION_MAJOR}")
  SET(CPACK_PACKAGE_VERSION_MINOR "${autowiring_VERSION_MINOR}")
  SET(CPACK_PACKAGE_VERSION_PATCH "${autowiring_VERSION_PATCH}")
  SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "autowiring")
  SET(CPACK_PACKAGE_INSTALL_DIRECTORY "autowiring")
  INCLUDE(CPack)
endif()