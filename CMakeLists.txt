cmake_minimum_required(VERSION 3.8)
include(version.cmake)
include("standard/Standard.cmake")

project(Autowiring VERSION ${autowiring_VERSION})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")
include(AddPCH)
include(ConditionalSources)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# We have unit test projects via googletest, they're added in the places where they are defined
enable_testing()

# Autoboost headers required everywhere but on MSVC 2015+, which doesn't rely on filesystem
if(NOT MSVC OR MSVC_VERSION LESS 1900)
  install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/contrib/autoboost/autoboost
    DESTINATION include
    COMPONENT autowiring
  )
endif()

# Enable conformance mode for newer versions of MSVC
if(MSVC_VERSION GREATER 1910)
  string(APPEND CMAKE_CXX_FLAGS " /permissive-")
endif()

add_definitions(-DGTEST_HAS_TR1_TUPLE=0)

# Recurse through source directories
include_directories(
  contrib
  contrib/websocketpp
)
add_subdirectory(contrib)
add_subdirectory(src)

# Now we can generate the version and install stuff
generate_version()
combined_installer(
  VENDOR "Leap Motion"
  CONTACT "cmercenary@gmail.com"
)
