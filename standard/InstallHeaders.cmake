# Installs headers for the named target
# Syntax:
# install_headers TARGET <target> DESTINATION <destination> ...
#
# NOEXCLUDE_STDAFX      Also install the precompiled header file
# <target>              The target whose sources are to be installed
# <destination>         The root of the destination folder where files will be copied
#
# Additional options are passed after FILES to the cmake install command
include(DefaultValue)

function(install_headers)
  set(options NOEXCLUDE_STDAFX)
  set(oneValueArgs TARGET DESTINATION)
  set(multiValueArgs)

  cmake_parse_arguments(opt "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  string(TOLOWER LOWERNAME CMAKE_PROJECT_NAME)
  default_value(opt_DESTINATION include/${LOWERNAME})

  if(NOT opt_TARGET)
    message(FATAL_ERROR "Cannot install files for a nonexistent target")
  endif()

  get_target_property(target_SRCS ${opt_TARGET} SOURCES)
  foreach(src IN LISTS target_SRCS)
    if(NOT ${opt_NOEXCLUDE_STDAFX} AND ${src} STREQUAL "stdafx.h")
      continue()
    endif()
    get_filename_component(src_ext ${src} EXT)
    if(NOT src_ext STREQUAL ".h")
      continue()
    endif()

    # Need to make the path absolute.  We first look in the source directory,
    # then we look in the binary directory, but only if the path is relative.
    if(NOT IS_ABSOLUTE ${src})
      get_filename_component(src_src ${src} ABSOLUTE BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
      get_filename_component(src_bin ${src} ABSOLUTE BASE_DIR ${CMAKE_CURRENT_BINARY_DIR})
      if(EXISTS ${src_src})
        set(src ${src_src})
        file(RELATIVE_PATH src_rel ${CMAKE_CURRENT_SOURCE_DIR} ${src})
        get_filename_component(src_rel ${src_rel} DIRECTORY)
        set(actual_rel ${src_rel})
      elseif(EXISTS ${src_bin})
        set(src ${src_bin})
        file(RELATIVE_PATH bin_rel ${CMAKE_CURRENT_BINARY_DIR} ${src})
        get_filename_component(bin_rel ${bin_rel} DIRECTORY)
        set(actual_rel ${bin_rel})
      else()
        message(FATAL_ERROR "Could not find input header file ${src}")
      endif()
    endif()



    get_filename_component(src_ext ${src} EXT)
    install(
      FILES ${src}
      DESTINATION ${opt_DESTINATION}/${actual_rel}
      ${opt_UNPARSED_ARGUMENTS}
    )
  endforeach()
endfunction()
