#.rst
# FindGlew
# ------------
#
# Created by Raffi Bedikian.
# Locate and configure Glew
#
# Interface Targets
# ^^^^^^^^^^^^^^^^^
#   FindGlew::FindGlew
#
# Variables
# ^^^^^^^^^
#   Glew_ROOT_DIR
#   Glew_FOUND
#   Glew_INCLUDE_DIR
#   Glew_LIBRARIES
#
find_path(Glew_ROOT_DIR
          NAMES include/GL/glew.h
          PATH_SUFFIXES glew-${Glew_FIND_VERSION}
                        Glew)
find_path(
    Glew_INCLUDE_DIR
    NAMES GL/glew.h
    HINTS ${Glew_ROOT_DIR}
    PATH_SUFFIXES include    
    NO_DEFAULT_PATH
    )
    
if(MSVC)
  find_library(Glew_LIBRARY_RELEASE "glew32s.lib" HINTS "${Glew_ROOT_DIR}" PATH_SUFFIXES lib)
  find_library(Glew_LIBRARY_DEBUG "glew32s.lib" HINTS "${Glew_ROOT_DIR}" PATH_SUFFIXES lib)
else()
  # Linux's glew-1.9.0 package's libs are in lib64
  find_library(Glew_LIBRARY_RELEASE "libGLEW.a" HINTS "${Glew_ROOT_DIR}" PATH_SUFFIXES lib lib64)
  find_library(Glew_LIBRARY_DEBUG "libGLEW.a" HINTS "${Glew_ROOT_DIR}" PATH_SUFFIXES lib lib64)
endif()
include(SelectConfigurations)
select_configurations(Glew LIBRARY LIBRARIES)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Glew DEFAULT_MSG Glew_ROOT_DIR Glew_INCLUDE_DIR Glew_LIBRARY_RELEASE Glew_LIBRARY_DEBUG)

include(CreateImportTargetHelpers)

generate_import_target(Glew STATIC)
