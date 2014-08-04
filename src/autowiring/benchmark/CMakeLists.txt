if(NOT AUTOWIRING_BUILD_BENCHMARKS)
  return()
endif()

set(AutowiringBenchmarkTest_SRCS
  AutowiringBenchmarkTest.hpp
  AutowiringBenchmarkTest.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutowiringBenchmarkTest_SRCS)
add_executable(AutowiringBenchmarkTest ${AutowiringBenchmarkTest_SRCS})
target_link_libraries(AutowiringBenchmarkTest Autowiring AutowiringFixture AutoTesting)
set_property(TARGET AutowiringBenchmarkTest PROPERTY FOLDER "Autowiring")

# This is a unit test, let CMake know this
add_test(NAME AutowiringBenchmarkTest COMMAND $<TARGET_FILE:AutowiringBenchmarkTest>)
