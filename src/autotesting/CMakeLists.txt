set(AutoTesting_SOURCES
  AutowiringEnclosure.h
  gtest-all-guard.h
  gtest-all-guard.cpp
)

rewrite_header_paths(AutoTesting_SOURCES)
add_library(AutoTesting ${AutoTesting_SOURCES})
set_property(TARGET AutoTesting PROPERTY FOLDER "Autowiring")
target_link_libraries(AutoTesting Autowiring)