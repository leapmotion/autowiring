include_directories(
  #include test helpers form autowiring
  ${PROJECT_SOURCE_DIR}/src/autowiring/test
)

set(AutoNetTest_SRCS
  AutoNetTest.cpp
  BreakpointTest.cpp
  WebsocketTest.cpp
)

add_pch(AutoNetTest_SRCS "stdafx.h" "stdafx.cpp")

add_executable(AutoNetTest ${AutoNetTest_SRCS})
target_link_libraries(AutoNetTest AutoTesting AutoNet Autowiring)
set_property(TARGET AutoNetTest PROPERTY FOLDER "Autowiring")

# This is a unit test, let CMake know this
add_test(NAME AutoNetTest COMMAND $<TARGET_FILE:AutoNetTest>)
