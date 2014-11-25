#.rst
# CreateImportTargetHelpers
# -------------------------
# 
# Created by Walter Gray
# A suite of functions helpful for writing Find modules that make use of the 
# INTERFACE targets and Usage Requirements concepts introduced in cmake 3.0.0.
# 
# The following functions are provided by this module:
# ::
#   map_var_to_prop
#   generate_import_target
#   
# ::
#     GENERATE_IMPORT_TARGET(<namespace> <type> [<target>])
#
#   namespace - The namespace of the library
#   type - The type of target.  Allowed values are SHARED, STATIC, INTERFACE and UNKNOWN
#   target - Optional parameter allowing you to specify the generated target name
#
# Generates an import target of the specified type with the default name
# <namespace>::<namespace>.
#
# The values for the resulting library are read from the standard variables below.
# Any variable which is found is marked as advanced.
# <namespace>_LIBRARY<_DEBUG/_RELEASE>
#   The file to use as the IMPORTED_LOCATION.  If <type> is SHARED, this should
#   be a .dll, .dylib, or .so.  If <type> is STATIC or UNKNOWN, this should be 
#   a .lib, or .a file.  This is unused for INTERFACE libraries
# <namespace>_SHARED_LIB<_DEBUG/_RELEASE>
#   The file to use as the IMPORTED_LOCATION if <type> is SHARED.  This is helpful
#   for backwards compatilbility where projects expect _LIBRARY to be the import_lib.
# <namespace>_IMPORT_LIB<_DEBUG/_RELEASE>
#   The import library corresponding to the .dll.  Only used on Windows. 
#   This may not use generator expressions.
# <namespace>_INTERFACE_LIBS<_DEBUG/_RELEASE>
#   Additional link libraries, *excluding* the <namespace>_LIBRARY and
#   <namespace>_IMPORT_LIB values, if any. Generator expressions OK.
# <namespace>_INCLUDE_DIR
#   Interface include directories. Generator expressions OK.
#
# ::
#     MAP_VAR_TO_PROP(target property variable [REQUIRED])
#
#   target - The name of the target we're changing.
#   property - The property to append to.
#   variable - The root name of the variable we're appending to target::property
#   REQUIRED - Optional arg, if specified will throw a fatal error if no matching variable is found
#
# Primarily used by GENERATE_IMPORT_TARGET, but perhaps helpful externally when dealing
# with tricky libraries.  Searches for <variable> as well as <variable>_<CONFIG> values,
# and appends any value found to the matching property (<variable>_DEBUG goes to <property>_DEBUG)

include(CMakeParseArguments)
include(VerboseMessage)

function(map_var_to_prop target property var )
  set(options REQUIRED)
  cmake_parse_arguments(map_var_to_prop ${options} "" "" ${ARGN})
  # message("Searching for ${var}")
  foreach(_config _DEBUG _RELEASE "")
    if(${var}${_config})
      verbose_message("Appending to ${target} ${property}${_config}: ${${var}${_config}}")
      set(_found TRUE)
      #mark_as_advanced(${property}${_config}) #anything that gets passed in here is an advanced variable
      set_property(TARGET ${target} APPEND 
        PROPERTY ${property}${_config} "${${var}${_config}}")
    endif()
  endforeach()
  
  #set the default to a nice generator expression if it is not in the blacklist
  set(_genexpr_unsupported_properties "IMPORTED_LOCATION")
  list(FIND _genexpr_unsupported_properties _find_result ${property})
  if(NOT ${var} AND (_find_result EQUAL -1))
    set(_defaultprop)
    foreach(_config DEBUG RELEASE)
      if(${var}_${_config})
        set(_defaultprop ${_defaultprop}$<$<CONFIG:${_config}>:${${var}_${_config}}>)
      endif()
    endforeach()
    set_property(TARGET ${target} PROPERTY ${property} ${_defaultprop})
  endif()

  if(map_var_to_prop_REQUIRED AND NOT _found)
    message(FATAL_ERROR "${target}: required variable ${var}${var_suffix} is undefined.")
  endif()
  
  get_target_property(_fullprop ${target} ${property})
  verbose_message("${target}:${property} = ${_fullprop}")
endfunction()

#acceptable libtypes are SHARED, STATIC, INTERFACE, and UNKNOWN
function(generate_import_target namespace libtype)
  set(_target ${namespace}::${namespace})
  cmake_parse_arguments(generate_import_target "LOCAL" "TARGET" "" ${ARGN})
  
  if(generate_import_target_TARGET)
    set(_target ${generate_import_target_TARGET})
  endif()
  
  if(${namespace}_FOUND)
    verbose_message("Generating ${libtype} lib: ${_target} with namespace ${namespace}")
    
    set(_global GLOBAL)
    if(generate_import_target_LOCAL)
      unset(_global)
    endif()
    
    add_library(${_target} ${libtype} IMPORTED ${_global})

    if(MSVC AND ${libtype} STREQUAL SHARED)
      map_var_to_prop(${_target} IMPORTED_IMPLIB ${namespace}_IMPORT_LIB REQUIRED)
    endif()        
    
    if(NOT ${libtype} STREQUAL INTERFACE)
      if(${libtype} STREQUAL SHARED AND (${namespace}_SHARED_LIB OR (${namespace}_SHARED_LIB_DEBUG AND ${namespace}_SHARED_LIB_RELEASE)))
        map_var_to_prop(${_target} IMPORTED_LOCATION ${namespace}_SHARED_LIB REQUIRED)
      else()
        map_var_to_prop(${_target} IMPORTED_LOCATION ${namespace}_LIBRARY REQUIRED)
      endif()
    endif()
    
    map_var_to_prop(${_target} INTERFACE_LINK_LIBRARIES ${namespace}_INTERFACE_LIBS) 
    map_var_to_prop(${_target} INTERFACE_INCLUDE_DIRECTORIES ${namespace}_INCLUDE_DIR)
  endif()
endfunction()