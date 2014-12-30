include_directories(
  # Need to make sure all of our tests can find gtest
  ${PROJECT_SOURCE_DIR}/contrib/gtest-1.7.0/fused-src
  ${PROJECT_SOURCE_DIR}/autowiring
  ${PROJECT_SOURCE_DIR}/src/autowiring
  ${PROJECT_SOURCE_DIR}/src/autotesting
)

add_subdirectory(autonet)
add_subdirectory(autowiring)
add_subdirectory(autotesting)
