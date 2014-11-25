#.rst
# FindBullet
# ------------
#
# Locate and configure Bullet Physics
#
# Interface Targets
# ^^^^^^^^^^^^^^^^^
#   FindBullet::FindBullet
#
# Variables
# ^^^^^^^^^
#   Bullet_ROOT_DIR
#   Bullet_FOUND
#   Bullet_INCLUDE_DIR
#   Bullet_LIBRARIES
#


find_path(Bullet_ROOT_DIR NAMES "src/btBulletDynamicsCommon.h" PATH_SUFFIXES bullet-2.82-r2704)

set(Bullet_INCLUDE_DIR ${Bullet_ROOT_DIR}/src)

if(MSVC)
  find_library(Bullet_COLLISION_LIBRARY_RELEASE NAMES "BulletCollision.lib" HINTS "${Bullet_ROOT_DIR}/lib/Win32/VS2013/Release" PATH_SUFFIXES lib)
  find_library(Bullet_DYNAMICS_LIBRARY_RELEASE NAMES "BulletDynamics.lib" HINTS "${Bullet_ROOT_DIR}/lib/Win32/VS2013/Release" PATH_SUFFIXES lib)
  find_library(Bullet_LINEAR_MATH_LIBRARY_RELEASE NAMES "LinearMath.lib" HINTS "${Bullet_ROOT_DIR}/lib/Win32/VS2013/Release" PATH_SUFFIXES lib)

  find_library(Bullet_COLLISION_LIBRARY_DEBUG NAMES "BulletCollision_Debug.lib" HINTS "${Bullet_ROOT_DIR}/lib/Win32/VS2013/Debug" PATH_SUFFIXES lib)
  find_library(Bullet_DYNAMICS_LIBRARY_DEBUG NAMES "BulletDynamics_Debug.lib" HINTS "${Bullet_ROOT_DIR}/lib/Win32/VS2013/Debug" PATH_SUFFIXES lib)
  find_library(Bullet_LINEAR_MATH_LIBRARY_DEBUG NAMES "LinearMath_Debug.lib" HINTS "${Bullet_ROOT_DIR}/lib/Win32/VS2013/Debug" PATH_SUFFIXES lib)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # Mac
  find_library(Bullet_COLLISION_LIBRARY_RELEASE NAMES "BulletCollision.a" HINTS "${Bullet_ROOT_DIR}/lib/Mac/Release" PATH_SUFFIXES lib)
  find_library(Bullet_DYNAMICS_LIBRARY_RELEASE NAMES "BulletDynamics.a" HINTS "${Bullet_ROOT_DIR}/lib/Mac/Release" PATH_SUFFIXES lib)
  find_library(Bullet_LINEAR_MATH_LIBRARY_RELEASE NAMES "LinearMath.a" HINTS "${Bullet_ROOT_DIR}/lib/Mac/Release" PATH_SUFFIXES lib)

  find_library(Bullet_COLLISION_LIBRARY_DEBUG NAMES "BulletCollision_Debug.a" HINTS "${Bullet_ROOT_DIR}/lib/Mac/Debug" PATH_SUFFIXES lib)
  find_library(Bullet_DYNAMICS_LIBRARY_DEBUG NAMES "BulletDynamics_Debug.a" HINTS "${Bullet_ROOT_DIR}/lib/Mac/Debug" PATH_SUFFIXES lib)
  find_library(Bullet_LINEAR_MATH_LIBRARY_DEBUG NAMES "LinearMath_Debug.a" HINTS "${Bullet_ROOT_DIR}/lib/Mac/Debug" PATH_SUFFIXES lib)
else()
  # No linux support from Bullet!
endif()

set(Bullet_LIBRARY_RELEASE "${Bullet_COLLISION_LIBRARY_RELEASE}" "${Bullet_DYNAMICS_LIBRARY_RELEASE}" "${Bullet_LINEAR_MATH_LIBRARY_RELEASE}")
set(Bullet_LIBRARY_DEBUG "${Bullet_COLLISION_LIBRARY_DEBUG}" "${Bullet_DYNAMICS_LIBRARY_DEBUG}" "${Bullet_LINEAR_MATH_LIBRARY_DEBUG}")
mark_as_advanced(Bullet_COLLISION_LIBRARY_RELEASE)
mark_as_advanced(Bullet_DYNAMICS_LIBRARY_RELEASE)
mark_as_advanced(Bullet_LINEAR_MATH_LIBRARY_RELEASE)
mark_as_advanced(Bullet_COLLISION_LIBRARY_DEBUG)
mark_as_advanced(Bullet_DYNAMICS_LIBRARY_DEBUG)
mark_as_advanced(Bullet_LINEAR_MATH_LIBRARY_DEBUG)

include(SelectConfigurations)
select_configurations(Bullet LIBRARY LIBRARIES)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Bullet DEFAULT_MSG Bullet_ROOT_DIR Bullet_INCLUDE_DIR Bullet_LIBRARY_RELEASE Bullet_LIBRARY_DEBUG)

include(CreateImportTargetHelpers)

generate_import_target(Bullet STATIC)
