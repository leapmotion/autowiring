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
    if(src_ext STREQUAL ".h")
      get_filename_component(src_rel ${src} DIRECTORY)
      install(
        FILES ${src}
        DESTINATION ${opt_DESTINATION}/${src_rel}
        ${opt_UNPARSED_ARGUMENTS}
      )
    endif()
  endforeach()
endfunction()
