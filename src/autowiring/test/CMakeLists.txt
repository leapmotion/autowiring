set(AutowiringTest_SRCS
  AutoPacketTest.cpp
  AutoPacketFactoryTest.cpp
  CommonUseCasesTest.cpp
  FactoryTest.cpp
  HasForwardOnlyType.cpp
  PostConstructTest.cpp
)

add_executable(AutowiringTest ${AutowiringTest_SRCS})
add_pch(AutowiringTest stdafx.h stdafx.cpp)
target_link_libraries(AutowiringTest Autowiring AutowiringFixture)
target_include_directories(AutowiringTest PRIVATE "${CMAKE_SOURCE_DIR}/contrib/autoboost")

# This is a unit test, let CMake know this
add_test(NAME AutowiringTest COMMAND $<TARGET_FILE:AutowiringTest>)
