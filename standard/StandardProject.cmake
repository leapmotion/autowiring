#[[
Simple project setup. There's a fair amount of boiler plate that has to happen
in a particular order around project setup. This function wraps project() and
does the following:

* Changes CMake around so that the output variables follow the /bin and /lib output
  directory convention popular on Gnu.
* Ensures that ARM and 64-bit code are built as position-independent code
* Verifies that the compiler actually supports C++11
* Sets the correct flags to enable C++11 on all platforms
* Ensures compilation happens with visibility set to hidden by default
* Builds fat binaries on mac
* Sets standard_BUILD* variables for consumption later down the line.
* Enforces the project has a VERSION set.
]]
function(standard_project project_name)
  cmake_parse_arguments(standard "" "VERSION" "LANGUAGES" ${ARGN})
  if(NOT standard_VERSION)
    message(FATAL_ERROR "Standard compliant projects must specify a version")
  endif()

  set(_language_arg "")
  if(standard_LANGUAGES)
    set(_language_arg LANGUAGES ${standard_LANGUAGES})
  endif()

  standard_project_preinit()
  project(${project_name} VERSION ${standard_VERSION} ${_language_arg})
  standard_project_postinit()
endfunction()

function(standard_project_preinit)
  # Pre-initialization steps - these variables must be set before the first call to
  # project()
  if(APPLE)
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
      set(CMAKE_OSX_ARCHITECTURES "arm" CACHE STRING "Mac OS X build architectures")
    else()
      # Build Fat binaries on OSX by default
      set(CMAKE_OSX_ARCHITECTURES "x86_64;i386" CACHE STRING "Mac OS X build architectures")
    endif()
  endif()

  #These do not strictly *have* to be set prior to project, but they can be so we will

  # Need to classify the architecture before we run anything else, this lets us easily configure the
  # find version file based on what the architecture was actually built to be
  if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
    set(standard_BUILD_ARM ON CACHE INTERNAL "")
    set(standard_BUILD_ARCHITECTURES "arm" CACHE INTERNAL "")
    set(standard_BUILD_64 OFF CACHE INTERNAL "")
  elseif(CMAKE_OSX_ARCHITECTURES STREQUAL "x86_64;i386")
    set(standard_BUILD_ARCHITECTURES x64 x86 CACHE INTERNAL "")
    set(standard_BUILD_64 ON CACHE INTERNAL "")
  elseif(CMAKE_SIZEOF_VOID_P STREQUAL 4)
    set(standard_BUILD_ARCHITECTURES "x86" CACHE INTERNAL "")
    set(standard_BUILD_64 OFF CACHE INTERNAL "")
  else()
    set(standard_BUILD_ARCHITECTURES "x64" CACHE INTERNAL "")
    set(standard_BUILD_64 ON CACHE INTERNAL "")
  endif()
  message(STATUS "Using architecture: ${standard_BUILD_ARCHITECTURES}")

   # All of our binaries go to one place:  The binaries output directory.  We only want to tinker
  # with this if we're building by ourselves, otherwise we just do whatever the enclosing project
  # wants us to do.
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin CACHE INTERNAL "Executable Output directory")

  # Libraries not only all wind up in the libraries directory, but we also keep them all together
  # here by putting them in the same place, regardless of whether they are debug or release.  This
  # makes globbing them together much easier.
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib CACHE INTERNAL "Shared Library output directory")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib CACHE INTERNAL "Shared Library output directory")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib CACHE INTERNAL "Shared Library output directory")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib CACHE INTERNAL "Static Library output directory")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib CACHE INTERNAL "Static Library output directory")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib CACHE INTERNAL "Static Library output directory")

  # Postfix on all debug libraries should be "d"
  set(CMAKE_DEBUG_POSTFIX d${CMAKE_DEBUG_POSTFIX} CACHE INTERNAL "Debug postfix")

  # 64-bit installations should suffix with 64 regardless of the CPU type (arm or intel)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    foreach(config IN LISTS CMAKE_CONFIGURATION_TYPES)
      string(TOUPPER ${config} config)
      set(CMAKE_${config}_POSTFIX "${CMAKE_${config}_POSTFIX}64" CACHE INTERNAL "$[config} Postfix")
    endforeach()
  endif()

  # We want -fPIC code for all 64-bit non-windows builds
  if(NOT WIN32)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE INTERNAL "Position-Independent Code")
  endif()
endfunction()

function(standard_project_postinit)
  ##Post-initialization steps. All of these depend on project() having been called.

  include(CTest)

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

  # Always use c++11 compiler with hidden visibility
  if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fvisibility=hidden" CACHE INTERNAL "C++ Compiler Flags")
  endif()

  # Clang needs special additional flags to build with C++11
  if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    message(STATUS "AppleClang C++11")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++" CACHE INTERNAL "C++ Compiler Flags")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++" CACHE INTERNAL "XCode Clang C++ STL Version")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "Clang C++11")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}-lstdc++" CACHE INTERNAL "C++ EXE Linker Flags")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "GCC C++11")
  endif()
endfunction()
