include_directories(
  # Need to make sure all of our tests can find gtest
  ${PROJECT_SOURCE_DIR}/contrib/gtest-1.7.0/fused-src
  ${PROJECT_SOURCE_DIR}/autowiring
)

if(APPLE)
  # Boost is actually required on Mac
  find_package(Boost REQUIRED)
else()
  find_package(Boost QUIET)
endif()

# If we can find boost, then go ahead and pull in its include directory
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIR})
endif()

add_subdirectory(autonet)
add_subdirectory(autowiring)
