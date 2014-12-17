if(NOT AUTOWIRING_BUILD_BENCHMARKS)
  return()
endif()

set(AutowiringBenchmarkTest_SRCS
  AutowiringBenchmarkTest.cpp
  CanBoostPriorityTest.cpp
)

add_pch(AutowiringBenchmarkTest_SRCS "stdafx.h" "stdafx.cpp")
add_executable(AutowiringBenchmarkTest ${AutowiringBenchmarkTest_SRCS})
target_link_libraries(AutowiringBenchmarkTest Autowiring AutowiringFixture AutoTesting)
set_property(TARGET AutowiringBenchmarkTest PROPERTY FOLDER "Autowiring")

# This is a unit test, let CMake know this
add_test(NAME AutowiringBenchmarkTest COMMAND $<TARGET_FILE:AutowiringBenchmarkTest>)
