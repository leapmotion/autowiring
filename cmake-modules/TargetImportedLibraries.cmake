#.rst:
# ImportTargetLibraries
# ---------------------
#
# Created by Walter Gray
# See CreateImportTargetHelpers.cmake for a suite of functions suitable for writing Find 
# modules compatible with this approach.
#
# ========================
# TARGET_IMPORTED_LIBRARIES(<target> <link_type> <import_target>)
#   Takes the same arguments as target_link_libraries, but for any listed library where
#   <import_target> is a valid target with a TYPE property of SHARED_LIBRARY, it will
#   read from the IMPORTED_LOCATION and IMPORTED_LOCATION_<CONFIG> parameters and generate
#   a custom post-build step to copy the shared library files to the appropriate location.
#   On windows, this is the TARGET_FILE_DIR of <target>.  link_type should be one of
#   PUBLIC, PRIVATE, or INTERFACE
#
#  TARGET_PACKAGE(<target> <package> ...)
#   Takes the same arguments as find_package, with the addition of the target you're
#   linking to as the first parameter.  Upon successfully finding the package, it
#   attempts to call TARGET_IMPORTED_LIBRARIES(<target> <package>::<package>)

include(CMakeParseArguments)
function(target_imported_libraries target)
  list(REMOVE_AT ARGV 0) #pop the target
  cmake_parse_arguments(target_imported_libraries "" "LINK_TYPE" "" ${ARGV})
  
  set(_library_list ${target_imported_libraries_UNPARSED_ARGUMENTS})
  target_link_libraries(${target} ${target_imported_libraries_LINK_TYPE} ${_library_list})

  #early out if the target isn't an EXECUTABLE
  get_target_property(_target_type ${target} TYPE)
  if(NOT ${_target_type} STREQUAL EXECUTABLE)
    return()
  endif()

  #setup custom commands to copy .dll/dylib files for all dependencies
  set(_lib_index 0 )
  list(LENGTH _library_list _lib_length)

  #foreach doesn't allow you to append to the list from within the loop.
  while(_lib_index LESS _lib_length)
    list(GET _library_list ${_lib_index} _import_lib)

    if(TARGET ${_import_lib})
      get_target_property(_depends ${_import_lib} INTERFACE_LINK_LIBRARIES)
      foreach(_depend ${_depends})
        if(TARGET ${_depend})
          list(FIND _library_list ${_depend} _found_lib)
          if(_found_lib EQUAL -1) #make sure we don't do duplicate adds.
            verbose_message("${target}:Adding nested dependency ${_depend}")
            list(APPEND _library_list ${_depend})
          else()
            verbose_message("${target}:skipping duplicate nested dependency ${_depend}")
          endif()

        endif()
      endforeach()

      get_target_property(_type ${_import_lib} TYPE)
      get_target_property(_imported ${_import_lib} IMPORTED)
      if(${_type} STREQUAL SHARED_LIBRARY AND ${_imported})
        
        set(_found_configs_expr)
        set(_imported_location)
        
        #if only the _<Config> variants are set, create a generator expression.
        get_target_property(_imported_location ${_import_lib} IMPORTED_LOCATION)
        if(NOT _imported_location)
          get_target_property(_imported_location_debug ${_import_lib} IMPORTED_LOCATION_DEBUG)
          get_target_property(_imported_location_release ${_import_lib} IMPORTED_LOCATION_RELEASE)
          if(NOT _imported_location_debug AND NOT _imported_location_release)
            message(FATAL_ERROR "No IMPORTED_LOCATION specified for SHARED import target ${_import_lib}")
          endif()
          set(_imported_location "$<$<CONFIG:DEBUG>:${_imported_location_debug}>$<$<CONFIG:RELEASE>:${_imported_location_release}>")
        endif()

        verbose_message("Adding copy command for ${_import_lib}: ${_imported_location}")

        if(MSVC)
          add_custom_command(TARGET ${target} POST_BUILD 
            COMMAND ${CMAKE_COMMAND} -E copy_if_different \"${_imported_location}\" \"$<TARGET_FILE_DIR:${target}>\")
        elseif(APPLE)
          get_target_property(_is_bundle ${target} MACOSX_BUNDLE)
          if(_is_bundle)
            add_custom_command(TARGET ${target} POST_BUILD
              COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target}>/../Frameworks/"
              COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_imported_location}" "$<TARGET_FILE_DIR:${target}>/../Frameworks/"
              COMMAND install_name_tool -change @loader_path/libLeap.dylib @loader_path/../Frameworks/libLeap.dylib "$<TARGET_FILE:${target}>")
            #call install_name_tool and fixup the dylib paths here:
          endif()
        else()
          message(WARNING "Automatic handling of shared libraries is unimplemented on this platform")
        endif()
      endif()
    endif()

    math(EXPR _lib_index "${_lib_index} + 1") #an extremely verbose i++...
    #if this is expensive, simply increment it when adding to the list.
    list(LENGTH _library_list _lib_length) #this is likely to have changed
  endwhile()
endfunction()

#This function wraps find_package, then calls target_imported_libraries on the generated package)
function(target_package target package )
  list(REMOVE_AT ARGV 0) # pop the target
  cmake_parse_arguments(target_package "" "LINK_TYPE" "" ${ARGV})

  if(TARGET ${package}::${package})
    verbose_message("${package}::${package} already exists, skipping find op")
  else()
    find_package(${target_package_UNPARSED_ARGUMENTS})
  endif()

  if(target_package_LINK_TYPE)
    target_imported_libraries(${target} ${package}::${package} LINK_TYPE ${target_package_LINK_TYPE})
  else()
    target_imported_libraries(${target} ${package}::${package})
  endif()
endfunction()
