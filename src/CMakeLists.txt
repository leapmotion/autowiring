include_directories(
  # Need to make sure all of our tests can find gtest
  ${PROJECT_SOURCE_DIR}/contrib/gtest-1.7.0/fused-src
  ${PROJECT_SOURCE_DIR}/autowiring
  ${PROJECT_SOURCE_DIR}/src/autowiring
)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

if(APPLE)
  # Boost is actually required on Mac
  find_package(Boost REQUIRED)
else()
  find_package(Boost)
endif()

# If we can find boost, then go ahead and pull in its include directory
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIR})
endif()

add_subdirectory(autonet)
add_subdirectory(autowiring)
add_subdirectory(autotesting)
