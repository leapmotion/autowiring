add_conditional_sources(
  Autoboost_SRCS
  "ON"
  GROUP_NAME "Autoboost"
  FILES
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/system/src/error_code.cpp
)

add_conditional_sources(
  Autoboost_SRCS
  "NOT MSVC"
  GROUP_NAME "Autoboost"
  FILES
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/chrono/src/process_cpu_clocks.cpp
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/chrono/src/thread_clock.cpp
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/chrono/src/chrono.cpp
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/thread/src/pthread/once_atomic.cpp
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/thread/src/pthread/thread.cpp
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/thread/src/future.cpp
)

add_conditional_sources(
  Autoboost_SRCS
  "ON"
  GROUP_NAME "Autoboost"
  FILES
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/filesystem/src/codecvt_error_category.cpp
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/filesystem/src/operations.cpp
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/filesystem/src/path.cpp
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/filesystem/src/path_traits.cpp
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/filesystem/src/portability.cpp
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/filesystem/src/unique_path.cpp
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/filesystem/src/utf8_codecvt_facet.cpp
)

add_conditional_sources(
  Autoboost_SRCS
  "MSVC"
  GROUP_NAME "Autoboost"
  FILES
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/filesystem/src/windows_file_codecvt.cpp
  ${PROJECT_SOURCE_DIR}/contrib/autoboost/libs/filesystem/src/windows_file_codecvt.hpp
)

add_library(Autoboost STATIC ${Autoboost_SRCS})

target_include_directories(
  Autoboost
  PUBLIC
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/contrib/autoboost>"
)

#
# Install library
#
install(TARGETS Autoboost EXPORT AutowiringTargets
  DESTINATION lib
  COMPONENT autowiring
  CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
)
install_headers(TARGET Autoboost DESTINATION include/autowiring COMPONENT autowiring)
