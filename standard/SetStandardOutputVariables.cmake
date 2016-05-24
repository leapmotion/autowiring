#[[
Simple global assignment utility for adjusting the default CMake output variables to follow
the required conventions for Leap Motion external libraries.  This function does the following:

* Changes CMake around so that the output variables follow the /bin and /lib output
  directory convention popular on Gnu.
* Ensures that ARM and 64-bit code are built as position-independent code
* Verifies that the compiler actually supports C++11
* Sets the correct flags to enable C++11 on all platforms
* Ensures compilation happens with visibility set to hidden by default
* Builds fat binaries on mac
]]
macro(set_standard_output_variables)
  if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;i386" CACHE STRING "Mac OS X build architectures" FORCE)
  endif()

  if(CMAKE_COMPILER_IS_GNUCC)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.8")
      message(FATAL_ERROR "GCC version 4.8 minimum is required to build ${CMAKE_PROJECT_NAME}")
    endif()
  elseif (MSVC)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "18.0")
      message(FATAL_ERROR "MSVC 2013 minimum is required to build ${CMAKE_PROJECT_NAME}")
    endif()
  endif()
  message(STATUS "Compiler version ${CMAKE_CXX_COMPILER_VERSION}")

  # Need to classify the architecture before we run anything else, this lets us easily configure the
  # find version file based on what the architecture was actually built to be
  if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
    set(standard_BUILD_ARM ON)
    set(standard_BUILD_ARCHITECTURES "arm")
    set(standard_BUILD_64 OFF)
  elseif(CMAKE_OSX_ARCHITECTURES STREQUAL "x86_64;i386")
    set(standard_BUILD_ARCHITECTURES x64 x86)
    set(standard_BUILD_64 ON)
  elseif(CMAKE_SIZEOF_VOID_P STREQUAL 4)
    set(standard_BUILD_ARCHITECTURES "x86")
    set(standard_BUILD_64 OFF)
  else()
    set(standard_BUILD_ARCHITECTURES "x64")
    set(standard_BUILD_64 ON)
  endif()
  message(STATUS "Using architecture: ${standard_BUILD_ARCHITECTURES}")

  # All of our binaries go to one place:  The binaries output directory.  We only want to tinker
  # with this if we're building by ourselves, otherwise we just do whatever the enclosing project
  # wants us to do.
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

  # Libraries not only all wind up in the libraries directory, but we also keep them all together
  # here by putting them in the same place, regardless of whether they are debug or release.  This
  # makes globbing them together much easier.
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)

  # Postfix on all debug libraries should be "d"
  set(CMAKE_DEBUG_POSTFIX d${CMAKE_DEBUG_POSTFIX})

  # 64-bit installations should suffix with 64 regardless of the CPU type (arm or intel)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    foreach(config IN LISTS CMAKE_CONFIGURATION_TYPES)
      string(TOUPPER ${config} config)
      string(CONCAT CMAKE_${config}_POSTFIX "${CMAKE_${config}_POSTFIX}" "64")
    endforeach()

    # We want position-independent code for all 64-bit non-windows builds
    if(NOT WIN32)
      list(APPEND CMAKE_CXX_FLAGS "-fPIC")
      list(APPEND CMAKE_C_FLAGS "-fPIC")
    endif()
  endif()

  # Always use c++11 compiler with hidden visibility
  if(NOT WIN32)
    list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -fvisibility=hidden")
  endif()

  # Clang needs special additional flags to build with C++11
  if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    message(STATUS "AppleClang C++11")
    list(APPEND CMAKE_CXX_FLAGS "-stdlib=libc++")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "Clang C++11")
    list(APPEND CMAKE_EXE_LINKER_FLAGS "-lstdc++")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "GCC C++11")
  endif()
endmacro()
