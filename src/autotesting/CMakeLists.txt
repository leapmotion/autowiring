set(AutoTesting_SOURCES
  AutowiringEnclosure.h
  gtest-all-guard.hpp
  gtest-all-guard.cpp
)

rewrite_header_paths(AutoTesting_SOURCES)
add_library(AutoTesting ${AutoTesting_SOURCES})
set_property(TARGET AutoTesting PROPERTY FOLDER "Autowiring")
target_link_libraries(AutoTesting Autowiring)
target_include_directories(AutoTesting PRIVATE "${CMAKE_SOURCE_DIR}/contrib/autoboost")

if(NOT AUTOWIRING_IS_EMBEDDED)
  install(TARGETS AutoTesting EXPORT AutowiringTargets
    DESTINATION lib
    COMPONENT autowiring
    CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
  )
endif()
