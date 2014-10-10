find_library(Boost REQUIRED)

include_directories(
  #include test helpers form autowiring
  ${PROJECT_SOURCE_DIR}/src/autowiring/test
)

set(AutoNetTest_SRCS
  AutoNetTest.cpp
  BreakpointTest.cpp
  WebsocketTest.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutoNetTest_SRCS)
link_directories(${Boost_LIBRARY_DIRS})
add_executable(AutoNetTest ${AutoNetTest_SRCS})
target_link_libraries(AutoNetTest AutoTesting AutoNet Autowiring ${Boost_LIBRARIES})
set_property(TARGET AutoNetTest PROPERTY FOLDER "Autowiring")

# This is a unit test, let CMake know this
add_test(NAME AutoNetTest COMMAND $<TARGET_FILE:AutoNetTest>)
