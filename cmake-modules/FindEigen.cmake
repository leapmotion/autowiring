#.rst
# FindEigen
# ----------
#
# Created by Walter Gray
# Locate and configure Eigen
#
# Interface Targets
# ^^^^^^^^^^^^^^^^^
#   Eigen::Eigen
#
# Variables
# ^^^^^^^^^
#   Eigen_ROOT_DIR
#   Eigen_FOUND
#   Eigen_INCLUDE_DIR

find_path(Eigen_ROOT_DIR
          NAMES Eigen/Eigen
          PATH_SUFFIXES eigen-${Eigen_FIND_VERSION}
                        eigen)

set(Eigen_INCLUDE_DIR "${Eigen_ROOT_DIR}")
if(EXISTS ${Eigen_INCLUDE_DIR})
  set(Eigen_FOUND)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Eigen DEFAULT_MSG Eigen_INCLUDE_DIR)

include(CreateImportTargetHelpers)
generate_import_target(Eigen INTERFACE)