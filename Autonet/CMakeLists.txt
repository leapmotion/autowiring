#
#   Create a library here
#

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ..
  )

SET(Autonet_SRCS
  AutonetViz.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" Autonet_SRCS)

add_library(Autonet ${Project_Libary_Mode} ${Autonet_SRCS})
set_target_properties(Autonet PROPERTIES STATIC_LIBRARY_FLAGS "${STATIC_LIB_FLAGS}")
target_link_libraries(Autonet Utility DataStructures Networking Websockets)
