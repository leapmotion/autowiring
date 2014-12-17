#.rst:
# SelectConfigurations
# ---------------------------
#
# Originally SelectConfigurations
# Modified by Walter gray to create generator expressions instead of
# using the optimized and debug keywords. This makes the output
# suitable for use with the INTERFACE_LINK_LIBRARIES property.
#
# select_configurations( basename )
#
# This macro takes a library base name as an argument, and will choose
# good values for basename_LIBRARY, basename_LIBRARIES,
# basename_LIBRARY_DEBUG, and basename_LIBRARY_RELEASE depending on what
# has been found and set.  If only basename_LIBRARY_RELEASE is defined,
# basename_LIBRARY will be set to the release value, and
# basename_LIBRARY_DEBUG will be set to basename_LIBRARY_DEBUG-NOTFOUND.
# If only basename_LIBRARY_DEBUG is defined, then basename_LIBRARY will
# take the debug value, and basename_LIBRARY_RELEASE will be set to
# basename_LIBRARY_RELEASE-NOTFOUND.
#
# If the generator supports configuration types, then basename_LIBRARY
# and basename_LIBRARIES will be set with debug and optimized flags
# specifying the library to be used for the given configuration.  If no
# build type has been set or the generator in use does not support
# configuration types, then basename_LIBRARY and basename_LIBRARIES will
# take only the release value, or the debug value if the release one is
# not set.

#=============================================================================
# Copyright 2009 Will Dicharry <wdicharry@stellarscience.com>
# Copyright 2005-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

# This macro was adapted from the FindQt4 CMake module and is maintained by Will
# Dicharry <wdicharry@stellarscience.com>.

function(select_configurations basename suffix)
  if(ARGC GREATER 2) #checking argv2 directly is bad, since the argvX variables are not reset between function calls
    set(_outvar ${ARGV2})
  else()
    set(_outvar ${suffix})
  endif()
  
  if(NOT ${basename}_${suffix}_RELEASE)
    set(${basename}_${suffix}_RELEASE "${basename}_${suffix}_RELEASE-NOTFOUND" CACHE FILEPATH "Path to a library.")
  endif()
  if(NOT ${basename}_${suffix}_DEBUG)
    set(${basename}_${suffix}_DEBUG "${basename}_${suffix}_DEBUG-NOTFOUND" CACHE FILEPATH "Path to a library.")
  endif()

  if( ${basename}_${suffix}_DEBUG AND ${basename}_${suffix}_RELEASE AND
      NOT ${basename}_${suffix}_DEBUG STREQUAL ${basename}_${suffix}_RELEASE AND
      ( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE ) )
      # if the generator supports configuration types or CMAKE_BUILD_TYPE
      # is set, then set optimized and debug options.
      set( ${basename}_${_outvar} "" )
      foreach( _libname IN LISTS ${basename}_${suffix}_RELEASE )
        list( APPEND ${basename}_${_outvar} $<$<CONFIG:RELEASE>:"${_libname}"> )
      endforeach()
      foreach( _libname IN LISTS ${basename}_${suffix}_DEBUG )
        list( APPEND ${basename}_${_outvar} $<$<CONFIG:DEBUG>:"${_libname}"> )
      endforeach()
  elseif( ${basename}_${suffix}_RELEASE )
    set( ${basename}_${_outvar} ${${basename}_${suffix}_RELEASE} )
  elseif( ${basename}_${suffix}_DEBUG )
    set( ${basename}_${_outvar} ${${basename}_${suffix}_DEBUG} )
  else()
    set( ${basename}_${_outvar} "${basename}_${suffix}-NOTFOUND")
  endif()
  
  #break out of local scope
  set(${basename}_${_outvar} ${${basename}_${_outvar}} PARENT_SCOPE)

  mark_as_advanced( ${basename}_${suffix}_RELEASE ${basename}_${suffix}_DEBUG )
endfunction()