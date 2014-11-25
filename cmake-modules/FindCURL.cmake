#.rst
# FindCURL
# ------------
#
# Created by Walter Gray.
# Locate and configure CURL
# 
# Interface Targets
# ^^^^^^^^^^^^^^^^^
#   CURL::CURL
#
# Variables
# ^^^^^^^^^
#  CURL_ROOT_DIR
#  CURL_FOUND
#  CURL_INCLUDE_DIR
#  CURL_LIBRARIES
#  CURL_LIBRARY_RELEASE
#  CURL_LIBRARY_DEBUG

find_path(CURL_ROOT_DIR
          NAMES include/curl/curl.h
          PATH_SUFFIXES curl-${CURL_FIND_VERSION}
                        curl)

set(CURL_INCLUDE_DIR ${CURL_ROOT_DIR}/include)

if(MSVC)
  find_library(CURL_LIBRARY_RELEASE "libcurl.lib" HINTS "${CURL_ROOT_DIR}/lib/release")
  find_library(CURL_LIBRARY_DEBUG "libcurl.lib" HINTS "${CURL_ROOT_DIR}/lib/debug")
else()
  find_library(CURL_LIBRARY_RELEASE "libcurl.a" HINTS "${CURL_ROOT_DIR}/lib")
  find_library(CURL_LIBRARY_DEBUG "libcurl.a" HINTS "${CURL_ROOT_DIR}/lib")
endif()
include(SelectConfigurations)
select_configurations(CURL LIBRARY LIBRARIES)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CURL DEFAULT_MSG CURL_INCLUDE_DIR CURL_LIBRARIES)

include(CreateImportTargetHelpers)
generate_import_target(CURL STATIC)