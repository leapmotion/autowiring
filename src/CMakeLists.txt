include_directories(
  # Need to make sure all of our tests can find gtest
  ${PROJECT_SOURCE_DIR}/contrib/gtest-1.7.0/fused-src
  ${PROJECT_SOURCE_DIR}/autowiring
  ${PROJECT_SOURCE_DIR}/src/autowiring
  ${PROJECT_SOURCE_DIR}/src/autotesting
)

# The boost C++11 shim is required when building without USE_LIBCXX
if(NOT USE_LIBCXX)
  find_package(Boost COMPONENTS thread atomic chrono system date_time QUIET)

  if (NOT Boost_FOUND)
    message(FATAL_ERROR "Boost is required when building without USE_LIBCXX")
  endif()

  include_directories(${Boost_INCLUDE_DIR})

  # Needed to link boost on windows
  if(WIN32)
    link_directories(${Boost_LIBRARY_DIRS})
  endif()
endif()

add_subdirectory(autonet)
add_subdirectory(autowiring)
add_subdirectory(autotesting)
