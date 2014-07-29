include_directories(
  #include test helpers form autowiring
  ${PROJECT_SOURCE_DIR}/src/autowiring/test
)

set(
  AutoNetTest_SRCS
  AutoNetServerTest.hpp
  AutoNetServerTest.cpp
  gtest-all-guard.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutoNetTest_SRCS)
add_executable(AutoNetTest ${AutoNetTest_SRCS})
target_link_libraries(AutoNetTest AutoNet)
set_property(TARGET AutoNetTest PROPERTY FOLDER "Autowiring")

# This is a unit test, let CMake know this
add_test(NAME AutoNetTest COMMAND $<TARGET_FILE:AutoNetTest>)
