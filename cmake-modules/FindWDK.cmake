# - Try to find WDK
# This module takes as inputs
#  WDK_ROOT_ALTERNATE - An alternate possible location for the wdk
#
# Once done this will define
#  WDK_FOUND - System has WDK
#  WDK_INCLUDE_DIRS - The WDK include directory
#  WDK_LIBRARIES - The libraries needed to use WDK
#  WDK_BIN - The path to the WDK binaries folder

set(wdkregpath80 "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots;KitsRoot]")
get_filename_component(wdkpath80 ${wdkregpath80} ABSOLUTE)

set(wdkregpath81 "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots;KitsRoot81]")
get_filename_component(wdkpath81 ${wdkregpath81} ABSOLUTE)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
 set(WDK_ARCH "x64")
else()
 set(WDK_ARCH "x86")
endif()

# Select a root path first:
find_path(
  WDK_ROOT_DIR Include/um/UPnP.h  
  HINTS ${wdkpath81} ${wdkpath80} ${WDK_ROOT_ALTERNATE}
)

# Generate include directories variable from the root path
set(WDK_INCLUDE_DIRS ${WDK_ROOT_DIR}/Include/um ${WDK_ROOT_DIR}/Include/Shared)

# Also generate the binaries directory:
find_path(
  WDK_BIN_DIR makecat.exe
  HINTS ${WDK_ROOT_DIR}/bin/${WDK_ARCH}
)

# Now we scan for all components:
foreach(COMPONENT ${WDK_FIND_COMPONENTS})
  string(TOUPPER ${COMPONENT} UPPERCOMPONENT)
  
  find_library(
    WDK_${UPPERCOMPONENT} ${COMPONENT}
    PATHS ${WDK_ROOT_DIR}
    PATH_SUFFIXES /Lib/win8/um/${WDK_ARCH} /Lib/winv6.3/um/${WDK_ARCH}
  )
  mark_as_advanced(CLEAR WDK_${UPPERCOMPONENT})
  list(APPEND WDK_LIBRARIES ${WDK_${UPPERCOMPONENT}})
endforeach()

include(FindPackageHandleStandardArgs)
if(WDK_FIND_COMPONENTS)
  find_package_handle_standard_args(WDK DEFAULT_MSG WDK_LIBRARIES WDK_INCLUDE_DIRS)
else()
  find_package_handle_standard_args(WDK DEFAULT_MSG WDK_INCLUDE_DIRS)
endif()


