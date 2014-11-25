#.rst
#LeapTemplates
#-------------
# Created by Walter Gray
#
# Some Leap Motion specific boilderplate code.
# Not reccomended for use outside of the Leap Motion engineering group,
# though you're welcome to define your own.

macro(leap_find_external_libraries pc_variant)
  find_path(EXTERNAL_LIBRARY_DIR "eigen-3.2.1/Eigen/CmakeLists.txt"
    PATHS
      "$ENV{EXTERNAL_LIBRARY_DIR}"
      "$ENV{LIBRARIES_PATH}"
      "$ENV{PATH}"
      "/opt/local/Libraries"
  )

  list(INSERT CMAKE_PREFIX_PATH 0 "${EXTERNAL_LIBRARY_DIR}")
endmacro()

macro(leap_use_standard_platform_settings)

  if(NOT (MSVC OR CMAKE_BUILD_TYPE))
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release." FORCE)
  endif()

  # Disable MinSizeRel & MaxSpeedRel
  set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  if(APPLE)
    if(NOT CMAKE_OSX_ARCHITECTURES)
      set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "Mac OS X build architectures" FORCE)
    endif()
    if(NOT CMAKE_OSX_SYSROOT)
      set(CMAKE_OSX_SYSROOT "macosx10.9" CACHE STRING "Mac OS X build environment" FORCE)
    endif()

    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.7" CACHE STRING "Mac OS X deployment target" FORCE)
    mark_as_advanced(CMAKE_OSX_DEPLOYMENT_TARGET)
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
    set(USE_LIBCXX ON)
  endif()

endmacro()
