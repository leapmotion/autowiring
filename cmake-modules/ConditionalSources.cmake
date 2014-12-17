#.rst
#ConditionalSources
#---------------
# Created by Walter Gray
# 
# This module defines a set of functions for specifying conditionally compiled source files,
# generally platform specific ones.  Files that are listed but whose condition is not met will
# still be visible in any IDE generated projects, but will not be compiled and will be isolated in
# a named filter group.  This is particularly useful for find and replace operations in multi-platform
# codebases.
#
# There are two groups of functions
# conditional_sources is sufficient for setting all the flags on a list of files so that they will be grouped
# and only compiled under the given conditions.
# add_conditional_sources will first call conditional_sources, and will then append the list of sources to the given
# variable.
#
# add_named_conditional_functions is used for defining shorthand functions. Examples are at the bottom, and cover the most common
# platform-specific use cases.
#
# It is also worth noting that this is the only example I'm aware of showing cmake macro for defining functions.
# The trick was realising that referencing ${ARGV} in a macro references the ARGV of the *macro*, but you can
# access the defined function's ARGV by putting "ARGV" in a variable, then double-dereferincing it - this prevents
# the ${ARGV} from being parsed by cmake's macro preprocessor.
include(VerboseMessage)

macro(_conditional_sources_parse_arguments)
  include(CMakeParseArguments)
  cmake_parse_arguments(conditional_sources "" "GROUP_NAME" "FILES" ${ARGN})

  if(NOT DEFINED conditional_sources_FILES)
    set(conditional_sources_FILES ${conditional_sources_UNPARSED_ARGUMENTS})
  endif()
endmacro()

function(conditional_sources condition_var)
  _conditional_sources_parse_arguments(${ARGN})

  source_group(${conditional_sources_GROUP_NAME} FILES ${conditional_sources_FILES})

  separate_arguments(condition_var)
  verbose_message("Evaluating conditional as: ${condition_var}")
  if(NOT (${condition_var}))
    set_source_files_properties( ${conditional_sources_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
    verbose_message("Setting INACTIVE source group \"${conditional_sources_GROUP_NAME}\" with files ${conditional_sources_FILES}")
  else()
    verbose_message("Setting source group \"${conditional_sources_GROUP_NAME}\" with files ${conditional_sources_FILES}")
  endif()
endfunction()

#as conditional_sources, but also appends the soruces to the source_list_var
function(add_conditional_sources source_list_var condition_var)
  list(REMOVE_AT ARGV 0)
  conditional_sources(${ARGV})

  _conditional_sources_parse_arguments(${ARGN})

  set(${source_list_var} ${${source_list_var}} ${conditional_sources_FILES} PARENT_SCOPE)
  verbose_message("Adding ${conditional_sources_FILES} to ${source_list_var}")
endfunction()

#defines 'func_name' and add_'func_name' shorthands for add_conditional_sources with pre-set conditions.
macro(add_named_conditional_functions func_name group_name condition)
  function(${func_name} ...)
    set(my_argv ARGV)
    conditional_sources("${condition}" GROUP_NAME "${group_name}" FILES ${${my_argv}})
  endfunction()

  function(add_${func_name} source_list_var ...)
    set(my_argv ARGV)

    list(REMOVE_AT ARGV 0)
    add_conditional_sources(${source_list_var} "${condition}" GROUP_NAME "${group_name}" FILES ${${my_argv}})
    set(${source_list_var} ${${source_list_var}} PARENT_SCOPE)
  endfunction()
endmacro()

#Some good defaults
add_named_conditional_functions("windows_sources" "Windows Source" WIN32)
add_named_conditional_functions("mac_sources" "Mac Source" APPLE)
add_named_conditional_functions("unix_sources" "Unix Source" "UNIX AND NOT WIN32")
add_named_conditional_functions("resource_files" "Resource Files" FALSE)
