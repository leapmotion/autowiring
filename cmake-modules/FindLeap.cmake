#.rst
# FindLeap
# ------------
#
# Created by Walter Gray.
# Locate and configure Leap
#
# Interface Targets
# ^^^^^^^^^^^^^^^^^
#   Leap::Leap
#
# Variables
# ^^^^^^^^^
#   Leap_ROOT_DIR
#   Leap_FOUND
#   Leap_INCLUDE_DIR
#   Leap_LIBRARY
#   Leap_IMPORT_LIB

find_path(Leap_ROOT_DIR
          NAMES include/Leap.h
          HINTS ${EXTERNAL_LIBRARY_DIR}
          PATH_SUFFIXES LeapSDK-${Leap_FIND_VERSION}
                        LeapSDK)
#we should check the version.txt file here...

set(Leap_INCLUDE_DIR "${Leap_ROOT_DIR}/include")
if(MSVC)
  find_library(Leap_IMPORT_LIB_RELEASE "Leap.lib" HINTS "${Leap_ROOT_DIR}/lib/x86")
  find_library(Leap_IMPORT_LIB_DEBUG "Leapd.lib" HINTS "${Leap_ROOT_DIR}/lib/x86")
  
  find_file(Leap_LIBRARY_RELEASE
            NAMES Leap.dll
            HINTS "${Leap_ROOT_DIR}/lib/x86")
  find_file(Leap_LIBRARY_DEBUG
            NAMES Leapd.dll
                  Leap.dll #fallback on the release library if we must
            HINTS "${Leap_ROOT_DIR}/lib/x86")
  mark_as_advanced(Leap_IMPORT_LIB_RELEASE Leap_IMPORT_LIB_DEBUG)
else()
  if(USE_LIBCXX)
    set(_libdir ${Leap_ROOT_DIR}/lib/libc++)
  else()
    set(_libdir ${Leap_ROOT_DIR}/lib)
  endif()
  
  find_library(Leap_LIBRARY_RELEASE
            NAMES libLeap.dylib
            HINTS "${_libdir}")
  find_library(Leap_LIBRARY_DEBUG
            NAMES libLeapd.dylib 
                  libLeap.dylib #fallback on the release library
            HINTS "${_libdir}")
endif()


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Leap DEFAULT_MSG Leap_ROOT_DIR Leap_INCLUDE_DIR Leap_LIBRARY_RELEASE Leap_LIBRARY_DEBUG)

include(CreateImportTargetHelpers)
generate_import_target(Leap SHARED)
