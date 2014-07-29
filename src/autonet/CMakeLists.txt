find_package(Boost QUIET)
if(NOT Boost_FOUND)
  message("Cannot build Autonet, boost not installed on this system")
  return()
endif()

add_googletest(test)
include_directories(
  ${Boost_INCLUDE_DIR}
)

set(AutoNet_SRCS
  AutoNetServer.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutoNet_SRCS)
add_library(AutoNet STATIC ${AutoNet_SRCS})
target_link_libraries(AutoNet jzon)
