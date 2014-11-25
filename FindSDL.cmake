#.rst:
# FindSDL
# -------
#
# Locate SDL library.  Modified by Walter Gray to be compatible with SDL 2.x and
# provide import library pseudo targets. Untested with SDL 1.x.  Then modified
# by Victor Dods to forget SDL 1.x, and only look for SDL 2.x.  This find module
# should be renamed to FindSDL2.cmake -- it is useful to note that the developers
# of SDL have actually changed the name of the library, not just the version.  It
# is actually "SDL2", not "SDL" anymore.  Thus a fully-qualified lib directory name
# would be something like "SDL2-2.0.3", and not "SDL-2.0.3" as one would expect.
#
# Imported Targets
# ^^^^^^^^^^^^^^^^
#   SDL::SDL
#     Basic import target. Use to build an application
#   SDL::Library
#     Advanced import target. Does not link with SDLmain, for use when building libraries
#   SDL::Main
#     Advanced import target. contains only SDLMain.lib
# Result Variables
# ^^^^^^^^^^^^^^^^
# This module defines the following variables
#
#   SDL_ROOT_DIR
#
#   SDL_FOUND
#
#   SDL_INCLUDE_DIR
#
#   SDL_LIBRARY
#     Where to find SDL.dll/dylib if linking dynamically, or .lib/.a statically
#   SDL_SHARED_LIB
#     Where to find the SDL.dll/dylib if it exists
#   SDL_STATIC_LIB
#     Where to find the SDL.lib/.a if it exists
#   SDL_IMPORT_LIB
#     Where to find SDL.lib (Windows only) if it exists
#   SDL_MAIN_LIBRARY
#     Where to find SDLmain.lib/.a
#   SDL_LINK_TYPE
#     Either STATIC or SHARED depending on if SDL.dll/dylib is found.  If both are available,
#     defaults to SHARED.  Setting this in the cache will attempt to force one or the other
#   SDL_VERSION_STRING
#     A human-readable string containing the version of SDL
#   SDL_LIBRARIES
#     A legacy string - contains a list of all .lib files used by SDL, modified by SDL_BUILDING_LIBRARY.
#
# This module responds to the flag:
#
# ::
#
#   SDL_BUILDING_LIBRARY
#     If this is defined, then no SDL_main will be linked in because
#     only applications need main().
#     Otherwise, it is assumed you are building an application and this
#     module will attempt to locate and set the proper link flags
#     as part of the returned SDL_LIBRARY variable.
#
#
#
# Don't forget to include SDLmain.h and SDLmain.m your project for the
# OS X framework based version.  (Other versions link to -lSDLmain which
# this module will try to find on your behalf.) Also for OS X, this
# module will automatically add the -framework Cocoa on your behalf.
#
#
#
# Additional Note: If you see an empty SDL_LIBRARY_TEMP in your
# configuration and no SDL_LIBRARY, it means CMake did not find your SDL
# library (SDL.dll, libsdl.so, SDL.framework, etc).  Set
# SDL_LIBRARY_TEMP to point to your SDL library, and configure again.
# Similarly, if you see an empty SDL_MAIN_LIBRARY, you should set this
# value as appropriate.  These values are used to generate the final
# SDL_LIBRARY variable, but when these values are unset, SDL_LIBRARY
# does not get created.
#
#
#
# $SDLDIR is an environment variable that would correspond to the
# ./configure --prefix=$SDLDIR used in building SDL.  l.e.galup 9-20-02
#
# Modified by Eric Wing.  Added code to assist with automated building
# by using environmental variables and providing a more
# controlled/consistent search behavior.  Added new modifications to
# recognize OS X frameworks and additional Unix paths (FreeBSD, etc).
# Also corrected the header search path to follow "proper" SDL
# guidelines.  Added a search for SDLmain which is needed by some
# platforms.  Added a search for threads which is needed by some
# platforms.  Added needed compile switches for MinGW.
#
# Modified by Walter Gray. Added code to find SDL2, and create an import
# target.
#
# On OSX, this will prefer the Framework version (if found) over others.
# People will have to manually change the cache values of SDL_LIBRARY to
# override this selection or set the CMake environment
# CMAKE_INCLUDE_PATH to modify the search paths.
#
# Note that the header path has changed from SDL/SDL.h to just SDL.h
# This needed to change because "proper" SDL convention is #include
# "SDL.h", not <SDL/SDL.h>.  This is done for portability reasons
# because not all systems place things in SDL/ (see FreeBSD).

#=============================================================================
# Copyright 2003-2009 Kitware, Inc.
# Copyright 2012 Benjamin Eikel
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

include(VerboseMessage)

function(find_multitype_library shared_out static_out import_out)
  list(REMOVE_AT ARGV 0) #remove shared_out
  list(REMOVE_AT ARGV 0) #remove static_out
  list(REMOVE_AT ARGV 0) #remove import_out

  set(_oldlibsuffixes "${CMAKE_FIND_LIBRARY_SUFFIXES}")
    set(CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_SHARED_LIBRARY_SUFFIX}")
    find_library(${shared_out} ${ARGV})
    set(CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_STATIC_LIBRARY_SUFFIX}")
    find_library(${static_out} ${ARGV})
    set(CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_IMPORT_LIBRARY_SUFFIX}")
    find_library(${import_out} ${ARGV})
  set(CMAKE_FIND_LIBRARY_SUFFIXES "${_oldlibsuffixes}")

  #TODO:verify the types of the static & import libraries
  if(MSVC)
    #file(READ ${static_out})
  endif()

endfunction()

#Checks <namespace>_SHARED_LIB, <namespace>_STATIC_LIB and <namespace>_IMPORT_LIB
#And fills <namespace>_LIBRARY with the appropriate lib type depending on which is found
#If both are found, it will default to shared.  We may, at a later time, also add verification
#of if the static library is actually a static lib and not an import lib on windows.
#It will then fill <namespace>_LIBRARY_TYPE with either SHARED or STATIC
function(select_library_type namespace)
  #select the primary library type
  if(${namespace}_SHARED_LIB AND EXISTS "${${namespace}_SHARED_LIB}")
    #add either the .lib or the .dylib to the libraries list
    if(${namespace}_IMPORT_LIB AND EXISTS "${${namespace}_IMPORT_LIB}")
      set(${namespace}_LIBRARIES "${${namespace}_LIBRARIES}" "${${namespace}_IMPORT_LIB}" PARENT_SCOPE)
    else()
      set(${namespace}_LIBRARIES "${${namespace}_LIBRARIES}" "${${namespace}_SHARED_LIB}" PARENT_SCOPE)
    endif()

    set(${namespace}_LIBRARY "${${namespace}_SHARED_LIB}" PARENT_SCOPE)
    set(${namespace}_LIBRARY_TYPE "SHARED" PARENT_SCOPE)
  elseif(${namespace}_STATIC_LIB AND EXISTS "${${namespace}_STATIC_LIB}")
    set(${namespace}_LIBRARIES "${${namespace}_LIBRARIES}" "${${namespace}_STATIC_LIB}" PARENT_SCOPE)
    set(${namespace}_LIBRARY "${${namespace}_STATIC_LIB}" PARENT_SCOPE)
    set(${namespace}_LIBRARY_TYPE "STATIC" PARENT_SCOPE)
  endif()

endfunction()

function(find_likely_dirs package dir_list_var path_list )
  list(REMOVE_AT ARGV 0) #pop package name
  list(REMOVE_AT ARGV 0) #pop dir_list_var

  set(_dirs ${${dir_list_var}}) #make sure we're appending

  foreach(_path ${ARGV})
    file(GLOB _subdirs RELATIVE ${_path} ${_path}/*)
    foreach(_subdir ${_subdirs})
      if(IS_DIRECTORY ${_path}/${_subdir} AND _subdir MATCHES "^${package}*")
        list(APPEND _dirs ${_path}/${_subdir})
      endif()
    endforeach()
  endforeach()

  set(${dir_list_var} "${_dirs}" PARENT_SCOPE)
endfunction()

function(sdl_parse_version_file filename major minor patch version_string)
  file(STRINGS "${filename}" _major_line REGEX "^#define[ \t]+SDL_MAJOR_VERSION[ \t]+[0-9]+$")
  file(STRINGS "${filename}" _minor_line REGEX "^#define[ \t]+SDL_MINOR_VERSION[ \t]+[0-9]+$")
  file(STRINGS "${filename}" _patch_line REGEX "^#define[ \t]+SDL_PATCHLEVEL[ \t]+[0-9]+$")
  string(REGEX REPLACE "^#define[ \t]+SDL_MAJOR_VERSION[ \t]+([0-9]+)$" "\\1" sdl_major "${_major_line}")
  string(REGEX REPLACE "^#define[ \t]+SDL_MINOR_VERSION[ \t]+([0-9]+)$" "\\1" sdl_minor "${_minor_line}")
  string(REGEX REPLACE "^#define[ \t]+SDL_PATCHLEVEL[ \t]+([0-9]+)$" "\\1" sdl_patch "${_patch_line}")
  unset(_major_line)
  unset(_minor_line)
  unset(_patch_line)

  set(${major} ${sdl_major} PARENT_SCOPE)
  set(${minor} ${sdl_minor} PARENT_SCOPE)
  set(${patch} ${sdl_patch} PARENT_SCOPE)
  set(${version_string} "${sdl_major}.${sdl_minor}.${sdl_patch}" PARENT_SCOPE)
endfunction()

#############################
# The actual find module begins here
#############################
if(NOT EXISTS SDL_ROOT_DIR)

  set(_likely_dirs "")

  # NOTE: this could be done much more cleanly if we write a multiple-return-value find_file and find_path.
  # As it stands, cmake's find_file and find_path functions return at most one value, even if there are
  # multiple matches, and which one it returns depends on a rather complicated prioritized list of dirs.
  
  #Find any dirs in the prefix path matching SDL* and add them to the list of candidates
  find_likely_dirs(SDL _likely_dirs "${CMAKE_PREFIX_PATH}")

  set(_best_version "")
  
  #TODO: create a filter function that takes a function(to determine version given a path)
  #and filters dirs based on the package version & if EXACT has been set.
  foreach(_dir ${_likely_dirs})
    find_path(
      _candidate_sdl2_root_dir
      NAMES include/SDL_version.h
            include/SDL2/SDL_version.h
            include/SDL/SDL_version.h
      HINTS $ENV{SDLDIR} ${_likely_dirs}
    )
    if(_candidate_sdl2_root_dir AND EXISTS ${_candidate_sdl2_root_dir})
      find_file(
        _version_file
        NAMES SDL_version.h
        PATHS ${_candidate_sdl2_root_dir}
        PATH_SUFFIXES include include/SDL2 include/SDL
        NO_DEFAULT_PATH
      )
      verbose_message("found SDL_version.h file in dir ${_candidate_sdl2_root_dir}")
      if(_version_file AND EXISTS ${_version_file})
        sdl_parse_version_file("${_version_file}" _major _minor _patch _version_string)
        verbose_message("    version string = ${_version_string}")

        #exact matches in front
        set(_version_matches FALSE)
        if(_version_string STREQUAL SDL_FIND_VERSION)
          set(_version_matches TRUE)
        endif()
        if(_major EQUAL SDL_FIND_VERSION_MAJOR AND
           _minor EQUAL SDL_FIND_VERSION_MINOR AND
           _patch EQUAL SDL_FIND_VERSION_PATCH)
          set(_version_matches TRUE)
        endif()
        if(NOT SDL_FIND_VERSION_EXACT AND
           _major EQUAL SDL_FIND_VERSION_MAJOR AND
           _version_string VERSION_GREATER SDL_FIND_VERSION)
          set(_version_matches TRUE)
        endif()

        if(_version_matches)
          verbose_message("    version ${_version_string} matches")
          if(NOT _best_version OR _version_string VERSION_GREATER _best_version)
            verbose_message("    setting best version")
            set(_best_version ${_version_string})
            set(SDL_ROOT_DIR ${_candidate_sdl2_root_dir})
            set(SDL_VERSION_STRING ${_version_string})
            set(SDL_VERSION_MAJOR ${_major})
            set(SDL_VERSION_MINOR ${_minor})
            set(SDL_VERSION_PATCH ${_patch})
          endif()
        endif()
      endif()
    endif()
  endforeach()
  verbose_message("SDL_ROOT_DIR = ${SDL_ROOT_DIR}")
  verbose_message("SDL_VERSION_STRING = ${SDL_VERSION_STRING}")
  verbose_message("SDL_VERSION_MAJOR = ${SDL_VERSION_MAJOR}")
  verbose_message("SDL_VERSION_MINOR = ${SDL_VERSION_MINOR}")
  verbose_message("SDL_VERSION_PATCH = ${SDL_VERSION_PATCH}")
  unset(_version_file CACHE)
  unset(_candidate_sdl2_root_dir CACHE)
endif()

### Find SDL version by using predefined SDL root directory ###
 find_file(
        _version_file
        NAMES SDL_version.h
        PATHS ${SDL_ROOT_DIR}
        PATH_SUFFIXES include include/SDL2 include/SDL
        NO_DEFAULT_PATH
      )
sdl_parse_version_file("${_version_file}" _major _minor _patch _version_string)
set(SDL_VERSION_MAJOR ${_major})

# A find_path command analogous to the one used to derived SDL_ROOT_DIR is used here.
find_path(
    SDL_INCLUDE_DIR
    NAMES SDL_version.h
    HINTS ${SDL_ROOT_DIR}
    PATH_SUFFIXES
        include/SDL2
        include
        include/SDL
    NO_DEFAULT_PATH
)
verbose_message("SDL_INCLUDE_DIR = ${SDL_INCLUDE_DIR}")

find_multitype_library(
  SDL_SHARED_LIB
  SDL_STATIC_LIB
  SDL_IMPORT_LIB
  NAMES
    SDL${SDL_VERSION_MAJOR}
  HINTS
    $ENV{SDLDIR} ${SDL_ROOT_DIR}
  PATH_SUFFIXES
    lib ${VC_LIB_PATH_SUFFIX}
  NO_DEFAULT_PATH
)

select_library_type(SDL)

find_library(SDL_MAIN_LIBRARY
  NAMES
    SDL${SDL_VERSION_MAJOR}main
    SDLmain
  HINTS
    ENV{SDLDIR} ${SDL_ROOT_DIR}
  PATH_SUFFIXES
    lib ${VC_LIB_PATH_SUFFIX}
  PATHS
    /sw /opt/local /opt/csw /opt
)

if(NOT SDL_BUILDING_LIBRARY)
  list(APPEND SDL_LIBRARIES ${SDL_MAIN_LIBRARY})
endif()

# MinGW needs an additional library, mwindows
# It's total link flags should look like -lmingw32 -lSDLmain -lSDL -lmwindows
# (Actually on second look, I think it only needs one of the m* libraries.)
if(MINGW)
  set(MINGW32_LIBRARY mingw32 CACHE STRING "mwindows for MinGW")
endif()

# For OS X, SDL uses Cocoa as a backend so it must link to Cocoa (as
# well as the dependencies of Cocoa (the frameworks: Carbon, IOKit,
# and the library: iconv)).  CMake doesn't display the -framework Cocoa
# string in the UI even though it actually is there if I modify a
# pre-used variable.  I think it has something to do with the CACHE
# STRING.  So I use a temporary variable until the end so I can set
# the "real" variable in one-shot.
if(APPLE)
  list(APPEND SDL_INTERFACE_LIBS "-framework Cocoa" "-framework IOKit"  "-framework Carbon" "iconv")
endif()

# For threads, as mentioned Apple doesn't need this.
# In fact, there seems to be a problem if I used the Threads package
# and try using this line, so I'm just skipping it entirely for OS X.
if(NOT APPLE)
  list(APPEND SDL_INTERFACE_LIBS ${CMAKE_THREAD_LIBS_INIT})
endif()

# For MinGW library
if(MINGW)
  list(APPEND SDL_INTERFACE_LIBS ${MINGW32_LIBRARY})
endif()

# Set the final string here so the GUI reflects the final state.
list(APPEND SDL_LIBRARIES ${SDL_INTERFACE_LIBS} CACHE STRING "Where the SDL Library can be found")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL
                                  REQUIRED_VARS SDL_LIBRARY SDL_MAIN_LIBRARY SDL_INCLUDE_DIR
                                  VERSION_VAR SDL_VERSION_STRING)

mark_as_advanced(SDL_INCLUDE_DIR SDL_LIBRARIES SDL_MAIN_LIBRARY SDL_IMPORT_LIB SDL_SHARED_LIB SDL_STATIC_LIB)

if(SDL_MAIN_LIBRARY AND EXISTS "${SDL_MAIN_LIBRARY}")
  set(SDL_MAIN_FOUND TRUE)
endif()

include(CreateImportTargetHelpers)

if(SDL_FOUND AND NOT TARGET SDL::SDL)
  generate_import_target(SDL_MAIN STATIC TARGET SDL::Main)

  include(CreateImportTargetHelpers)
  if(SDL_LIBRARY MATCHES "${CMAKE_SHARED_LIBRARY_SUFFIX}$")
    generate_import_target(SDL SHARED TARGET SDL::Library)
  elseif(SDL_LIBRARY MATCHES "${CMAKE_STATIC_LIBRARY_SUFFIX}$")
    generate_import_target(SDL STATIC TARGET SDL::Library)
  else()
    message(FATAL_ERROR "Unable to determine library type of file ${SDL_LIBRARY}")
  endif()

  add_library(SDL::SDL INTERFACE IMPORTED GLOBAL)
  set_property(TARGET SDL::SDL APPEND PROPERTY INTERFACE_LINK_LIBRARIES SDL::Library SDL::Main)

  # NOTE: this is commented out because SDL does not in principle need to depend
  # on X11 (it should be using Cocoa), and thus the SDL library we use should be
  # configured to not use X11.  Jon has rolled an SDL build with X11 disabled, and
  # that build should be making it into our external libraries.
  # # HACK FOR MAC X11 DEPENDENCY
  # # TODO - Create a modernized FindX11.cmake module, make SDL depend on it on macs
  # if(APPLE)
  #   find_package(X11 REQUIRED)
  #   set_property(TARGET SDL::SDL APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${X11_INCLUDE_DIR})
  # endif()
endif()