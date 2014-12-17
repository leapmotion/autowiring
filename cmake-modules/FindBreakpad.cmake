#.rst
# FindBreakpad
# ------------
#
# Created by Walter Gray.
# Locate and configure Breakpad
#
# Interface Targets
# ^^^^^^^^^^^^^^^^^
#   Breakpad::Breakpad
#
# Variables
# ^^^^^^^^^
#   Breakpad_FOUND
#   Breakpad_INCLUDE_DIR
#   Breakpad_LIBRARY_DIR
#   Breakpad_ROOT_DIR

find_path(Breakpad_ROOT_DIR
          NAMES include/google_breakpad/common/breakpad_types.h
          PATH_SUFFIXES breakpad-${Breakpad_FIND_VERSION}
                        breakpad)

set(Breakpad_INCLUDE_DIR "${Breakpad_ROOT_DIR}/include")
set(Breakpad_LIBRARY_DIR "${Breakpad_ROOT_DIR}/lib")
foreach(_lib IN ITEMS common.lib exception_handler.lib crash_generation_client.lib)
  list(APPEND Breakpad_LIBRARIES "${Breakpad_LIBRARY_DIR}/${_lib}")
endforeach()

set(Breakpad_LIBRARY ${Breakpad_LIBRARIES})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Breakpad DEFAULT_MSG Breakpad_ROOT_DIR Breakpad_INCLUDE_DIR Breakpad_LIBRARY_DIR Breakpad_LIBRARIES)

include(CreateImportTargetHelpers)
generate_import_target(Breakpad STATIC)
