#.rst
# FindNanoSVG
# ----------
#
# Created by Jonathan Marsden
# Locate and configure nanosvg
#
# Interface Targets
# ^^^^^^^^^^^^^^^^^
#   NanoSVG::NanoSVG
#
# Variables
# ^^^^^^^^^
#   NanoSVG_ROOT_DIR
#   NanoSVG_FOUND
#   NanoSVG_INCLUDE_DIR

find_path(NanoSVG_ROOT_DIR
          NAMES include/nanosvg.h
          PATH_SUFFIXES nanosvg)

set(NanoSVG_INCLUDE_DIR "${NanoSVG_ROOT_DIR}/include")
if(EXISTS ${NanoSVG_INCLUDE_DIR})
  set(NanoSVG_FOUND)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NanoSVG DEFAULT_MSG NanoSVG_INCLUDE_DIR)

include(CreateImportTargetHelpers)
generate_import_target(NanoSVG INTERFACE)
