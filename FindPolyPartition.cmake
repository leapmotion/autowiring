#.rst
# FindPolyPartition
# ------------
#
# Created by Jonathan Marsden
# Locate and configure PolyPartition
#
# Interface Targets
# ^^^^^^^^^^^^^^^^^
#   PolyPartition::PolyPartition
#
# Variables
# ^^^^^^^^^
#   PolyPartition_ROOT_DIR
#   PolyPartition_FOUND
#   PolyPartition_INCLUDE_DIR
#   PolyPartition_LIBRARY
#   PolyPartition_IMPORT_LIB

find_path(PolyPartition_ROOT_DIR
          NAMES include/polypartition.h
          HINTS ${EXTERNAL_LIBRARY_DIR}
          PATH_SUFFIXES polypartition)

set(PolyPartition_INCLUDE_DIR "${PolyPartition_ROOT_DIR}/include")
if(MSVC)
  find_library(PolyPartition_LIBRARY_RELEASE "polypartition.lib" HINTS "${PolyPartition_ROOT_DIR}/lib/release")
  find_library(PolyPartition_LIBRARY_DEBUG "polypartition.lib" HINTS "${PolyPartition_ROOT_DIR}/lib/debug")
else()
  find_library(PolyPartition_LIBRARY_RELEASE "libpolypartition.a" HINTS "${PolyPartition_ROOT_DIR}/lib")
  find_library(PolyPartition_LIBRARY_DEBUG "libpolypartition.a" HINTS "${PolyPartition_ROOT_DIR}/lib")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PolyPartition DEFAULT_MSG PolyPartition_ROOT_DIR PolyPartition_INCLUDE_DIR PolyPartition_LIBRARY_RELEASE PolyPartition_LIBRARY_DEBUG)

include(CreateImportTargetHelpers)
generate_import_target(PolyPartition STATIC)
