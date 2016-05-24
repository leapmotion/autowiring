cmake_minimum_required(VERSION 3.4)
include(version.cmake)
project(Autowiring VERSION ${autowiring_VERSION})
include("standard/Standard.cmake")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")
include(AddPCH)
include(ConditionalSources)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Standard calls
set_standard_output_variables()

# We have unit test projects via googletest, they're added in the places where they are defined
enable_testing()

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
