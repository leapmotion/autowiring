include_directories(
  #include test helpers form autowiring
  ${PROJECT_SOURCE_DIR}/src/autowiring/test
)

set(AutoNetTest_SRCS
  AutoNetTest.cpp
  BreakpointTest.cpp
  RPCTest.cpp
  WebsocketTest.cpp
)

add_executable(AutoNetTest ${AutoNetTest_SRCS})
add_pch(AutoNetTest "stdafx.h" "stdafx.cpp")
target_link_libraries(AutoNetTest AutoTesting AutoNet Autowiring)

# This is a unit test, let CMake know this
add_test(NAME AutoNetTest COMMAND $<TARGET_FILE:AutoNetTest>)
