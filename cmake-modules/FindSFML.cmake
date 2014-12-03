# This script locates the SFML library
# ------------------------------------
#
# Usage
# -----
#
# When you try to locate the SFML libraries, you must specify which modules you want to use (system, window, graphics, network, audio, main).
# If none is given, the SFML_LIBRARIES variable will be empty and you'll end up linking to nothing.
# example:
#   find_package(SFML COMPONENTS graphics window system) // find the graphics, window and system modules
#
# You can enforce a specific version, either MAJOR.MINOR or only MAJOR.
# If nothing is specified, the version won't be checked (ie. any version will be accepted).
# example:
#   find_package(SFML COMPONENTS ...)     // no specific version required
#   find_package(SFML 2 COMPONENTS ...)   // any 2.x version
#   find_package(SFML 2.4 COMPONENTS ...) // version 2.4 or greater
#
# By default, the dynamic libraries of SFML will be found. To find the static ones instead,
# you must set the SFML_STATIC_LIBRARIES variable to TRUE before calling find_package(SFML ...).
# In case of static linking, the SFML_STATIC macro will also be defined by this script.
# example:
#   set(SFML_STATIC_LIBRARIES TRUE)
#   find_package(SFML 2 COMPONENTS network system)
#
# On Mac OS X if SFML_STATIC_LIBRARIES is not set to TRUE then by default CMake will search for frameworks unless
# CMAKE_FIND_FRAMEWORK is set to "NEVER" for example. Please refer to CMake documentation for more details.
# Moreover, keep in mind that SFML frameworks are only available as release libraries unlike dylibs which
# are available for both release and debug modes.
#
# If SFML is not installed in a standard path, you can use the SFML_ROOT CMake (or environment) variable
# to tell CMake where SFML is.
#
# Output
# ------
#
# This script defines the following interface targets:
#   SFML::
#           SFML - Meta-target.  Links to all targets below.
#           Main - The minimum required core target.
#           Audio
#           Graphics
#           Network
#           System
#           Window
#           
#   Targets for different components will only defined for modules in the COMPONENTS list passed to find_package.
#   If no components are listed, it is assumed that all components are requested.
#
# This script also defines the following variables:
# - For each specified module XXX (system, window, graphics, network, audio, main):
#   - SFML_XXX_LIBRARY_DEBUG:   the name of the debug library of the xxx module (set to SFML_XXX_LIBRARY_RELEASE is no debug version is found)
#   - SFML_XXX_LIBRARY_RELEASE: the name of the release library of the xxx module (set to SFML_XXX_LIBRARY_DEBUG is no release version is found)
#   - SFML_XXX_LIBRARY:         the name of the library to link to for the xxx module (includes both debug and optimized names if necessary)
#   - SFML_XXX_FOUND:           true if either the debug or release library of the xxx module is found
# - SFML_LIBRARIES:   the list of all libraries corresponding to the required modules
# - SFML_DEFINITIONS:     thie list of compiler flags required.
# - SFML_FOUND:       true if all the required modules are found
# - SFML_INCLUDE_DIR: the path where SFML headers are located (the directory containing the SFML/Config.hpp file)
#
# example:
#   find_package(SFML 2 COMPONENTS system window graphics audio REQUIRED)
#   include_directories(${SFML_INCLUDE_DIR})
#   add_executable(myapp ...)
#   target_link_libraries(myapp ${SFML_LIBRARIES})

# define the SFML_STATIC macro if static build was chosen
set(SFML_DEFINITIONS "")

if(SFML_STATIC_LIBRARIES)
    set(SFML_DEFINITIONS SFML_STATIC)
endif()

find_path(SFML_ROOT_DIR
          NAMES include/SFML/Config.hpp
          PATH_SUFFIXES SFML_${SFML_FIND_VERSION}_${_compiler_SUFFIX}
                        SFML_${SFML_FIND_VERSION}
                        SFML-${SFML_FIND_VERSION}-${_compiler_SUFFIX}
                        SFML-${SFML_FIND_VERSION}
                        SFML)

# deduce the libraries suffix from the options
set(FIND_SFML_LIB_SUFFIX "")
if(SFML_STATIC_LIBRARIES)
    set(FIND_SFML_LIB_SUFFIX "${FIND_SFML_LIB_SUFFIX}-s")
endif()

# find the SFML include directory
find_path(SFML_INCLUDE_DIR SFML/Config.hpp
          PATH_SUFFIXES include
          PATHS
          ${SFML_ROOT}
          $ENV{SFML_ROOT}
          ${SFML_ROOT_DIR}
          ~/Library/Frameworks
          /Library/Frameworks
          /usr/local/
          /usr/
          /sw          # Fink
          /opt/local/  # DarwinPorts
          /opt/csw/    # Blastwave
          /opt/)
mark_as_advanced(SFML_INCLUDE_DIR)

# check the version number
set(SFML_VERSION_OK TRUE)
if(SFML_FIND_VERSION AND SFML_INCLUDE_DIR)
    # extract the major and minor version numbers from SFML/Config.hpp
    # we have to handle framework a little bit differently :
    if("${SFML_INCLUDE_DIR}" MATCHES "SFML.framework")
        set(SFML_CONFIG_HPP_INPUT "${SFML_INCLUDE_DIR}/Headers/Config.hpp")
    else()
        set(SFML_CONFIG_HPP_INPUT "${SFML_INCLUDE_DIR}/SFML/Config.hpp")
    endif()
    FILE(READ "${SFML_CONFIG_HPP_INPUT}" SFML_CONFIG_HPP_CONTENTS)
    STRING(REGEX MATCH ".*#define SFML_VERSION_MAJOR ([0-9]+).*#define SFML_VERSION_MINOR ([0-9]+).*" SFML_CONFIG_HPP_CONTENTS "${SFML_CONFIG_HPP_CONTENTS}")
    STRING(REGEX REPLACE ".*#define SFML_VERSION_MAJOR ([0-9]+).*" "\\1" SFML_VERSION_MAJOR "${SFML_CONFIG_HPP_CONTENTS}")
    STRING(REGEX REPLACE ".*#define SFML_VERSION_MINOR ([0-9]+).*" "\\1" SFML_VERSION_MINOR "${SFML_CONFIG_HPP_CONTENTS}")
    math(EXPR SFML_REQUESTED_VERSION "${SFML_FIND_VERSION_MAJOR} * 10 + ${SFML_FIND_VERSION_MINOR}")

    # if we could extract them, compare with the requested version number
    if (SFML_VERSION_MAJOR)
        # transform version numbers to an integer
        math(EXPR SFML_VERSION "${SFML_VERSION_MAJOR} * 10 + ${SFML_VERSION_MINOR}")

        # compare them
        if(SFML_VERSION LESS SFML_REQUESTED_VERSION)
            set(SFML_VERSION_OK FALSE)
        endif()
    else()
        # SFML version is < 2.0
        if (SFML_REQUESTED_VERSION GREATER 19)
            set(SFML_VERSION_OK FALSE)
            set(SFML_VERSION_MAJOR 1)
            set(SFML_VERSION_MINOR x)
        endif()
    endif()
endif()

# find the requested modules
set(SFML_FOUND TRUE) # will be set to false if one of the required modules is not found
set(FIND_SFML_LIB_PATHS
    ${SFML_ROOT}
    $ENV{SFML_ROOT}
    ${SFML_ROOT_DIR}
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local
    /usr
    /sw
    /opt/local
    /opt/csw
    /opt)

if(NOT SFML_FIND_COMPONENTS)
  set(SFML_FIND_COMPONENTS audio graphics network system window)
  if(WIN32)
    list(APPEND SFML_FIND_COMPONENTS main)
  endif()
endif()

foreach(FIND_SFML_COMPONENT ${SFML_FIND_COMPONENTS})
    string(TOLOWER ${FIND_SFML_COMPONENT} FIND_SFML_COMPONENT_LOWER)
    string(TOUPPER ${FIND_SFML_COMPONENT} FIND_SFML_COMPONENT_UPPER)
    set(FIND_SFML_COMPONENT_NAME sfml-${FIND_SFML_COMPONENT_LOWER}${FIND_SFML_LIB_SUFFIX})

    # no suffix for sfml-main, it is always a static library
    if(FIND_SFML_COMPONENT_LOWER STREQUAL "main")
        set(FIND_SFML_COMPONENT_NAME sfml-${FIND_SFML_COMPONENT_LOWER})
    endif()

    # debug library
    find_library(SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_DEBUG
                 NAMES ${FIND_SFML_COMPONENT_NAME}-d
                 PATH_SUFFIXES lib64 lib
                 PATHS ${FIND_SFML_LIB_PATHS})

    # release library
    find_library(SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_RELEASE
                 NAMES ${FIND_SFML_COMPONENT_NAME}
                 PATH_SUFFIXES lib64 lib
                 PATHS ${FIND_SFML_LIB_PATHS})

    if(NOT SFML_STATIC_LIBRARIES)
      set(SFML_${FIND_SFML_COMPONENT_UPPER}_IMPORT_LIB_DEBUG ${SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_DEBUG})
      set(SFML_${FIND_SFML_COMPONENT_UPPER}_IMPORT_LIB_RELEASE ${SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_RELEASE})

      set(_old_suffixes ${CMAKE_FIND_LIBRARY_SUFFIXES})
      set(CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_SHARED_LIBRARY_SUFFIX}")
      find_library(SFML_${FIND_SFML_COMPONENT_UPPER}_SHARED_LIB_DEBUG
                   NAMES ${FIND_SFML_COMPONENT_NAME}-d ${FIND_SFML_COMPONENT_NAME}-d-2
                   PATH_SUFFIXES bin64 bin
                   PATHS ${FIND_SFML_LIB_PATHS})

      find_library(SFML_${FIND_SFML_COMPONENT_UPPER}_SHARED_LIB_RELEASE
                   NAMES ${FIND_SFML_COMPONENT_NAME} ${FIND_SFML_COMPONENT_NAME}-2
                   PATH_SUFFIXES bin64 bin
                   PATHS ${FIND_SFML_LIB_PATHS})

      set(CMAKE_FIND_LIBRARY_SUFFIXES ${_old_suffixes})
      mark_as_advanced(SFML_${FIND_SFML_COMPONENT_UPPER}_SHARED_LIB_DEBUG SFML_${FIND_SFML_COMPONENT_UPPER}_SHARED_LIB_RELEASE)
    endif()

    if (SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_DEBUG OR SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_RELEASE)
        # library found
        set(SFML_${FIND_SFML_COMPONENT_UPPER}_FOUND TRUE)
        
        # if both are found, set SFML_XXX_LIBRARY to contain both
        if (SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_DEBUG AND SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_RELEASE)
            set(SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY debug     ${SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_DEBUG}
                                                          optimized ${SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_RELEASE})
        endif()

        # if only one debug/release variant is found, set the other to be equal to the found one
        if (SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_DEBUG AND NOT SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_RELEASE)
            # debug and not release
            set(SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_RELEASE ${SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_DEBUG})
            set(SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY         ${SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_DEBUG})
        endif()
        if (SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_RELEASE AND NOT SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_DEBUG)
            # release and not debug
            set(SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_DEBUG ${SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_RELEASE})
            set(SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY       ${SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_RELEASE})
        endif()

    else()
        # library not found
        set(SFML_FOUND FALSE)
        set(SFML_${FIND_SFML_COMPONENT_UPPER}_FOUND FALSE)
        set(SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY "")
        set(FIND_SFML_MISSING "${FIND_SFML_MISSING} SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY")
    endif()

    # mark as advanced
    MARK_AS_ADVANCED(SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY
                     SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_RELEASE
                     SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY_DEBUG)

    # add to the global list of libraries
    set(SFML_LIBRARIES ${SFML_LIBRARIES} "${SFML_${FIND_SFML_COMPONENT_UPPER}_LIBRARY}")
endforeach()

# handle errors
if(NOT SFML_VERSION_OK)
    # SFML version not ok
    set(FIND_SFML_ERROR "SFML found but version too low (requested: ${SFML_FIND_VERSION}, found: ${SFML_VERSION_MAJOR}.${SFML_VERSION_MINOR})")
    set(SFML_FOUND FALSE)
elseif(NOT SFML_FOUND)
    # include directory or library not found
    set(FIND_SFML_ERROR "Could NOT find SFML (missing: ${FIND_SFML_MISSING})")
endif()
if (NOT SFML_FOUND)
    if(SFML_FIND_REQUIRED)
        # fatal error
        message(FATAL_ERROR ${FIND_SFML_ERROR})
    elseif(NOT SFML_FIND_QUIETLY)
        # error but continue
        message("${FIND_SFML_ERROR}")
    endif()
endif()

# handle success
#if(SFML_FOUND)
    #message(STATUS "Found SFML ${SFML_VERSION_MAJOR}.${SFML_VERSION_MINOR} in ${SFML_INCLUDE_DIR}")
#endif()

include(CreateImportTargetHelpers)

if( SFML_FOUND AND NOT TARGET SFML::SFML)

  set(_libtype SHARED)
  if(SFML_STATIC_LIBRARIES)
    set(_libtype STATIC)
  endif()


  add_library(SFML::SFML INTERFACE IMPORTED GLOBAL)

  if(APPLE)
    find_library(SFML_GLEW
                 NAMES GLEW
                 PATH_SUFFIXES lib64 lib
                 PATHS ${FIND_SFML_LIB_PATHS}
                 NO_DEFAULT_PATH)
    find_library(SFML_JPEG
                 NAMES jpeg
                 PATH_SUFFIXES lib64 lib
                 PATHS ${FIND_SFML_LIB_PATHS}
                 NO_DEFAULT_PATH)
    mark_as_advanced(SFML_GLEW SFML_JPEG)
  endif()

  foreach(_component ${SFML_FIND_COMPONENTS})
    string(TOUPPER ${_component} _componentUPPER)
    string(TOLOWER ${_component} _componentLOWER)

    string(SUBSTRING ${_componentLOWER} 0 1 _first_letter)
    string(TOUPPER ${_first_letter} _first_letter)
    string(REGEX REPLACE "^.(.*)" "${_first_letter}\\1" _componentCap "${_componentLOWER}")

    if(_componentLOWER STREQUAL "main") #main is always a static lib
      generate_import_target(SFML_MAIN STATIC TARGET SFML::Main)
      
    else()
      generate_import_target(SFML_${_componentUPPER} ${_libtype} TARGET SFML::${_componentCap})
    endif()

    map_var_to_prop(SFML::${_componentCap} INTERFACE_COMPILE_DEFINITIONS SFML_DEFINITIONS)
    map_var_to_prop(SFML::${_componentCap} INTERFACE_INCLUDE_DIRECTORIES SFML_INCLUDE_DIR REQUIRED)

    if(APPLE)
      if(SFML_STATIC_LIBRARIES)
        if(_componentLOWER STREQUAL "audio")
          set_property(TARGET SFML::${_componentCap} APPEND PROPERTY INTERFACE_LINK_LIBRARIES "-framework OpenAL")
        endif()
        if(_componentLOWER STREQUAL "graphics")
          set_property(TARGET SFML::${_componentCap} APPEND PROPERTY
            INTERFACE_LINK_LIBRARIES "-framework OpenGL" "-framework AppKit" "-framework IOKit" "-framework Carbon"
                                     "${SFML_GLEW}" "${SFML_JPEG}")
        endif()
      endif()
    endif()

    set_property(TARGET SFML::SFML APPEND PROPERTY INTERFACE_LINK_LIBRARIES SFML::${_componentCap})
  endforeach()
endif()