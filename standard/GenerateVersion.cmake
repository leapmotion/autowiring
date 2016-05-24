# Generator function for $projectname-config.cmake and $projectname-configVersion.cmake
# 
# This generator is used to create a version locator file compatible with find_package
# that is compliant with the SemVer standard and also architecture-aware.  This ensures
# that find_package for your 32-bit build will not ever accidentally bring in the 64-bit
# build, and that find_packge for your 0.3.2 release will never accidentally find 0.4.0
# as a forwards-compatible version, or that 2.0.0 is not considered a replacement for
# 1.0.0
#
# Usage:
# GenerateVersion(
#    [DIRECTORY DirectoryName]   # Directory where the version files will be written, defaults to CMAKE_BINARY_DIR
#    [NAME ProjectName]          # The project name, if different from CMAKE_PROJECT_NAME
#    [VERSION Version]           # The version, if different from PROJECT_VERSION
# )
set(SELF ${CMAKE_CURRENT_LIST_DIR})
include(DefaultValue)
include(ParseVersion)

function(generate_version)
  set(options )
  set(oneValueArgs DIRECTORY NAME)
  set(multiValueArgs )
  
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  
  default_value(ARG_DIRECTORY ${CMAKE_BINARY_DIR})
  default_value(ARG_NAME ${CMAKE_PROJECT_NAME})
  default_value(ARG_VERSION ${${ARG_NAME}_VERSION})
  parse_version(ARG_VERSION ${ARG_VERSION})
  string(TOLOWER ${ARG_NAME} ARG_NAME_LOWER)
  
  # Need to classify the architecture before we run anything else, this lets us easily configure the
  # find version file based on what the architecture was actually built to be
  if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
    set(standard_BUILD_ARM ON)
    set(standard_BUILD_ARCHITECTURES "arm")
    set(standard_BUILD_64 OFF)
  elseif(CMAKE_OSX_ARCHITECTURES STREQUAL "x86_64;i386")
    set(standard_BUILD_ARCHITECTURES x64 x86)
    set(standard_BUILD_64 ON)
  elseif(CMAKE_SIZEOF_VOID_P STREQUAL 4)
    set(standard_BUILD_ARCHITECTURES "x86")
    set(standard_BUILD_64 OFF)
  else()
    set(standard_BUILD_ARCHITECTURES "x64")
    set(standard_BUILD_64 ON)
  endif()
  message(STATUS "Using architecture: ${standard_BUILD_ARCHITECTURES}")
  
  configure_file(${SELF}/standard-config.cmake.in ${ARG_DIRECTORY}/${ARG_NAME_LOWER}-config.cmake @ONLY)
  configure_file(${SELF}/standard-configVersion.cmake.in ${ARG_DIRECTORY}/${ARG_NAME_LOWER}-configVersion.cmake @ONLY)
  
  # Install ${ARG_NAME}-config.cmake and ${ARG_NAME}-configVersion.cmake
  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${ARG_NAME}-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${ARG_NAME}-configVersion.cmake"
    DESTINATION "."
    COMPONENT ${CMAKE_PROJECT_NAME}
  )
  
  # Export library
  export(
    EXPORT ${ARG_NAME}Targets
    FILE ${ARG_NAME}Targets.cmake
    NAMESPACE ${ARG_NAME}::
  )

  # Targets file is needed in order to describe how to link ${ARG_NAME} to the rest of the system
  install(
    EXPORT ${ARG_NAME}Targets
    FILE ${ARG_NAME}Targets.cmake
    COMPONENT ${ARG_NAME}
    NAMESPACE ${ARG_NAME}::
    DESTINATION cmake
    CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
  )
endfunction()
