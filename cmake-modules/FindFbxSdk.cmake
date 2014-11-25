#.rst
# FindFbxSdk
# ------------
#
# Created by Walter Gray.
# Locate and configure FbxSdk
#
# Interface Targets
# ^^^^^^^^^^^^^^^^^
#   FbxSdk::FbxSdk
#
# Variables
# ^^^^^^^^^
#   FbxSdk_ROOT_DIR
#   FbxSdk_FOUND
#   FbxSdk_INCLUDE_DIR
#   FbxSdk_LIBRARIES

find_path(FbxSdk_ROOT_DIR
          NAMES include/fbxsdk.h
          HINTS ${EXTERNAL_LIBRARY_DIR}
          PATH_SUFFIXES fbx-sdk-${FbxSdk_FIND_VERSION}
                        fbx-sdk
                        # NOTE: 2014.2 does not compile with VS2012
                        # fbx-sdk/2014.2
                        # TODO: we should make this folder structure more consistent, most likely fbx-sdk/2015.1
                        fbx-sdk/2015.1
                        fbx2015/2015.1)

set(FbxSdk_INCLUDE_DIR "${FbxSdk_ROOT_DIR}/include")

if (CMAKE_SIZEOF_VOID_P EQUAL 8) # 64bit
  set(BUILD_BIT_TYPE "x64")
else() # 32bit
  set(BUILD_BIT_TYPE "x86")
endif()

if(MSVC)
  find_library(FbxSdk_LIBRARY_RELEASE "libfbxsdk-md.lib" HINTS "${FbxSdk_ROOT_DIR}/lib/vs2013/${BUILD_BIT_TYPE}/release")
  find_library(FbxSdk_LIBRARY_DEBUG "libfbxsdk-md.lib" HINTS "${FbxSdk_ROOT_DIR}/lib/vs2013/${BUILD_BIT_TYPE}/debug")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux") # This is the correct way to detect Linux operating system -- see http://www.openguru.com/2009/04/cmake-detecting-platformoperating.html
  find_library(FbxSdk_LIBRARY_RELEASE "libfbxsdk.a" HINTS "${FbxSdk_ROOT_DIR}/lib/gcc4/${BUILD_BIT_TYPE}/release")
  find_library(FbxSdk_LIBRARY_DEBUG "libfbxsdk.a" HINTS "${FbxSdk_ROOT_DIR}/lib/gcc4/${BUILD_BIT_TYPE}/debug")
else()
  find_library(FbxSdk_LIBRARY_RELEASE "libfbxsdk.a" HINTS "${FbxSdk_ROOT_DIR}/lib/clang/ub/release")
  find_library(FbxSdk_LIBRARY_DEBUG "libfbxsdk.a" HINTS "${FbxSdk_ROOT_DIR}/lib/clang/ub/debug")
endif()
include(SelectConfigurations)
select_configurations(FbxSdk LIBRARY LIBRARIES)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FbxSdk DEFAULT_MSG FbxSdk_INCLUDE_DIR FbxSdk_LIBRARIES)

include(CreateImportTargetHelpers)
generate_import_target(FbxSdk STATIC)

