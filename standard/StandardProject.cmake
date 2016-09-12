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

include (CMakeParseArguments) #Backwards compatibilty

#This must be a macro since project defines scope-local variables
#that we generally rely on being in the root context.
macro(standard_project project_name)
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
endmacro()

function(standard_project_preinit)
  # Pre-initialization steps - these variables must be set before the first call to
  # project()
  if(APPLE AND CMAKE_OSX_ARCHITECTURES STREQUAL "")
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
      set(CMAKE_OSX_ARCHITECTURES "arm" CACHE STRING "Mac OS X build architectures" FORCE)
    else()
      # Build Fat binaries on OSX by default
      set(CMAKE_OSX_ARCHITECTURES "x86_64;i386" CACHE STRING "Mac OS X build architectures" FORCE)
    endif()
  endif()

  #These do not strictly *have* to be set prior to project, but they can be so we will

  # Need to classify the architecture before we run anything else, this lets us easily configure the
  # find version file based on what the architecture was actually built to be
  if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
    set(standard_BUILD_ARM ON PARENT_SCOPE)
    set(standard_BUILD_ARCHITECTURES "arm" PARENT_SCOPE)
    set(standard_BUILD_64 OFFPARENT_SCOPE)
  elseif(CMAKE_OSX_ARCHITECTURES STREQUAL "x86_64;i386")
    set(standard_BUILD_ARCHITECTURES x64 x86 PARENT_SCOPE)
    set(standard_BUILD_64 ON PARENT_SCOPE)
  elseif(CMAKE_SIZEOF_VOID_P STREQUAL 4)
    set(standard_BUILD_ARCHITECTURES "x86" PARENT_SCOPE)
    set(standard_BUILD_64 OFF PARENT_SCOPE)
  else()
    set(standard_BUILD_ARCHITECTURES "x64" PARENT_SCOPE)
    set(standard_BUILD_64 ON PARENT_SCOPE)
  endif()
  message(STATUS "Using architecture: ${standard_BUILD_ARCHITECTURES}")

   # All of our binaries go to one place:  The binaries output directory.  We only want to tinker
  # with this if we're building by ourselves, otherwise we just do whatever the enclosing project
  # wants us to do.
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin PARENT_SCOPE)

  # Libraries not only all wind up in the libraries directory, but we also keep them all together
  # here by putting them in the same place, regardless of whether they are debug or release.  This
  # makes globbing them together much easier.
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib PARENT_SCOPE)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib PARENT_SCOPE)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib PARENT_SCOPE)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib PARENT_SCOPE)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib PARENT_SCOPE)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib PARENT_SCOPE)

  # Postfix on all debug libraries should be "d"
  # PARENT_SCOPE causes the variable to not be set locally, and we
  # rely on this value in the following loop....

  set(CMAKE_DEBUG_POSTFIX "d${CMAKE_DEBUG_POSTFIX}")
  set(CMAKE_DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX} PARENT_SCOPE)

  # 64-bit installations should suffix with 64 regardless of the CPU type (arm or intel)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    foreach(config IN LISTS CMAKE_CONFIGURATION_TYPES)
      string(TOUPPER ${config} config)
      set(CMAKE_${config}_POSTFIX "${CMAKE_${config}_POSTFIX}64" PARENT_SCOPE)
    endforeach()
  endif()

  # Standard environment - Position independent code, Symbols hidden by default, and standard C++11
  set(CMAKE_POSITION_INDEPENDENT_CODE ON PARENT_SCOPE)
  set(CMAKE_CXX_VISIBILITY_PRESET hidden PARENT_SCOPE)
  set(CMAKE_VISIBILITY_INLINES_HIDDEN ON PARENT_SCOPE)
  set(CMAKE_CXX_STANDARD 11 PARENT_SCOPE)
  set(CMAKE_CXX_STANDARD_REQUIRED ON PARENT_SCOPE)
  set(CMAKE_CXX_EXTENSIONS OFF PARENT_SCOPE)

  #CMAKE_OSX_DEPLOYMENT_TARGET < 10.9 implies -stdlib=libstdc++, which doesn't have
  #complete c++11 support. override with libc++
  if(DEFINED CMAKE_OSX_DEPLOYMENT_TARGET AND CMAKE_OSX_DEPLOYMENT_TARGET VERSION_LESS 10.9)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++" PARENT_SCOPE)
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

endfunction()
