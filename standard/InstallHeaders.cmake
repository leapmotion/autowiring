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

    #When the path to the file is absolute, we don't know what we should be installing
    #it relative to. Warn
    if(IS_ABSOLUTE ${src})
      message(WARNING "Not sure where to install file with absolute path ${src}")
      continue()
    endif()

    #Determine which path the file is relative to and use intermediate directories as the install path
    foreach(_search_dir "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")
      get_filename_component(src_abs ${src} ABSOLUTE BASE_DIR ${_search_dir})
      if(EXISTS ${src_abs})
        set(src ${src_abs})
        file(RELATIVE_PATH containing_dir ${_search_dir} ${src})
        get_filename_component(containing_dir ${containing_dir} DIRECTORY)
        break()
      endif()
    endforeach()

    if(NOT EXISTS ${src_abs})
      message(FATAL_ERROR "Could not find input header file ${src}")
    endif()

    install(
      FILES ${src}
      DESTINATION ${opt_DESTINATION}/${containing_dir}
      ${opt_UNPARSED_ARGUMENTS}
    )
  endforeach()
endfunction()
