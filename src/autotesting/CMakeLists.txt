set(AutoTesting_SOURCES
  AutowiringEnclosure.h
  gtest-all-guard.hpp
  gtest-all-guard.cpp
)

add_library(AutoTesting ${AutoTesting_SOURCES})
target_link_libraries(AutoTesting Autowiring)
target_include_directories(
  AutoTesting
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/contrib/autoboost
)
install_headers(TARGET AutoTesting DESTINATION include/autotesting COMPONENT autowiring)

if(NOT AUTOWIRING_IS_EMBEDDED)
  install(TARGETS AutoTesting EXPORT AutowiringTargets
    DESTINATION lib
    COMPONENT autowiring
    CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
  )
endif()
