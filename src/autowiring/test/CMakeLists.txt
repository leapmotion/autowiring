set(AutowiringTest_SRCS
  CommonUseCasesTest.cpp
  HasForwardOnlyType.cpp
)

add_executable(AutowiringTest ${AutowiringTest_SRCS})
add_pch(AutowiringTest stdafx.h stdafx.cpp)
target_link_libraries(AutowiringTest Autowiring AutoTesting)

# This is a unit test, let CMake know this
add_test(NAME AutowiringTest COMMAND $<TARGET_FILE:AutowiringTest>)
