#.rst
# FindCinder
# ----------
#
# Created by Walter Gray
# Locate and configure cinder 0.8.5 for vc2010. Untested with other versions.
#
# Interface Targets
# ^^^^^^^^^^^^^^^^^
#   Cinder::Cinder
#     Interface target. references Cinder::Core and all
#	    required boost libraries with the required ordering.
#
#   Cinder::Core
#     Static import target. Does not include cinder's dependencies.
#     Typically not referenced outside of Cinder::Cinder
#
# Variables
# ^^^^^^^^^
#   Cinder_ROOT_DIR
#     Root of the cinder package
#   Cinder_FOUND
#     If false, do not link to Cinder
#   Cinder_LIBRARIES
#     The names of the libraries to feed into target_link_libraries
#   Cinder_INCLUDE_DIR
#     Where to find the root of the cinder includes
#   Cinder_LIBRARY_<CONFIG>
#     The location of the main cinder library
#
# This module will also find and add the Boost package which is included with Cinder,
# create a master Boost::Boost interface library which links to a series of 
# Boost::<component> static import libraries.

if(MSVC10)
  set(_compiler_SUFFIX "vc2010")
endif()

find_path(Cinder_ROOT_DIR
          NAMES include/cinder/Cinder.h
          PATH_SUFFIXES cinder_${Cinder_FIND_VERSION}_${_compiler_SUFFIX}
                        cinder_${Cinder_FIND_VERSION}
                        cinder)


find_path(Cinder_INCLUDE_DIR 
          NAMES "cinder/Cinder.h"
          HINTS "${Cinder_ROOT_DIR}"
          PATH_SUFFIXES "include")

find_library(Cinder_LIBRARY_RELEASE "cinder.lib" HINTS "${Cinder_ROOT_DIR}/lib")
find_library(Cinder_LIBRARY_DEBUG "cinder_d.lib" HINTS "${Cinder_ROOT_DIR}/lib")

include(SelectConfigurations)
select_configurations(Cinder LIBRARY LIBRARIES)

#Find Boost
if(MSVC)
  set(BOOST_LIBRARYDIR "${Cinder_ROOT_DIR}/lib/msw")
  set(_boost_components chrono date_time filesystem regex system thread)
else()
  set(BOOST_LIBRARYDIR "${Cinder_ROOT_DIR}/lib/macosx")
  set(_boost_components date_time filesystem system)
endif()


set(BOOST_ROOT ${Cinder_ROOT_DIR}/boost)
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost REQUIRED QUIET COMPONENTS ${_boost_components} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Cinder DEFAULT_MSG Cinder_ROOT_DIR Cinder_LIBRARIES Cinder_INCLUDE_DIR)

#Setup the interface targets

#Thanks to cinder's custom boost layout, we have to first create import targets
#for all of the boost components, then link them with the Cinder master interface library
#BEFORE linking in the actual cinder import target
include(CreateImportTargetHelpers)

if(Cinder_FOUND AND NOT TARGET Cinder::Cinder)
  generate_import_target(Boost INTERFACE)
  foreach(_component ${_boost_components})
    string(TOUPPER ${_component} _componentUPPER)
    generate_import_target(Boost_${_componentUPPER} STATIC TARGET Boost::${_component})
    target_link_libraries(Boost::Boost INTERFACE Boost::${_component})
  endforeach()

  generate_import_target(Cinder STATIC TARGET Cinder::Core)
  add_library(Cinder::Cinder INTERFACE IMPORTED GLOBAL)
  target_link_libraries(Cinder::Cinder INTERFACE Boost::Boost Cinder::Core)
endif()