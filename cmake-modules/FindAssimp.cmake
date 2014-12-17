#.rst
# FindAssimp
# ------------
#
# Created by Raffi Bedikian.
# Locate and configure Assimp
#
# Interface Targets
# ^^^^^^^^^^^^^^^^^
#   Assimp::Assimp
#
# Variables
# ^^^^^^^^^
#   Assimp_ROOT_DIR
#   Assimp_FOUND
#   Assimp_INCLUDE_DIR
#   Assimp_LIBRARIES
#

find_path(Assimp_ROOT_DIR
          NAMES include/assimp/scene.h
          PATH_SUFFIXES assimp-${Assimp_FIND_VERSION}
                        assimp)

set(Assimp_INCLUDE_DIR ${Assimp_ROOT_DIR}/include)

if(MSVC)
  find_library(Assimp_LIBRARY_RELEASE "assimp.lib" HINTS "${Assimp_ROOT_DIR}" PATH_SUFFIXES lib)
  find_library(Assimp_LIBRARY_DEBUG "assimpd.lib" HINTS "${Assimp_ROOT_DIR}" PATH_SUFFIXES lib)
else(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  find_library(Assimp_LIBRARY_RELEASE "libassimp.a" HINTS "${Assimp_ROOT_DIR}" PATH_SUFFIXES lib)
  find_library(Assimp_LIBRARY_DEBUG "libassimp.a" HINTS "${Assimp_ROOT_DIR}" PATH_SUFFIXES lib)
endif()
include(SelectConfigurations)
select_configurations(Assimp LIBRARY LIBRARIES)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Assimp DEFAULT_MSG Assimp_ROOT_DIR Assimp_INCLUDE_DIR Assimp_LIBRARY_RELEASE Assimp_LIBRARY_DEBUG)

include(CreateImportTargetHelpers)

generate_import_target(Assimp STATIC)

