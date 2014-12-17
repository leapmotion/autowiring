#.rst
#
# VerboseMessage
# --------------
#
# Handy function for printing debug info
function(verbose_message ...)
  if(CMAKE_VERBOSE_MAKEFILE OR VERBOSE)
    message(STATUS "${ARGV}")
  endif()
endfunction()

# Prints a list of vars in the format
# varname1 = "${varname1}", varname2 = "${varname2}", etc.
function(verbose_message_print_vars ...)
  set(_output_string "")
  foreach(_var_name ${ARGV})
      set(_output_string "${_output_string}${_var_name} = \"${${_var_name}}\", ")
  endforeach()
  verbose_message("${_output_string}")
endfunction()
