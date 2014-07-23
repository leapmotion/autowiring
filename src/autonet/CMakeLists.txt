OPTION(BUILD_AUTONET "Enable autonet features of autowiring project." OFF)

if(NOT BUILD_AUTONET)
  return()
endif()

include_directories(${Boost_INCLUDE_DIR})

set(AutoNet_SRCS
  AutoNetServer.h
  AutoNetServer.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" AutoNet_SRCS)
add_library(AutoNet STATIC ${AutoNet_SRCS})
target_link_libraries(AutoNet debug websocketpp_debug)
target_link_libraries(AutoNet optimized websocketpp_release)

add_library(websocketpp STATIC IMPORTED)
if(WIN32)
  set_property(TARGET websocketpp PROPERTY IMPORTED_LOCATION ${WEBSOCKETPP_DIR}/lib/websocketpp.lib)
else() # Mac or Linux
  set_property(TARGET websocketpp PROPERTY IMPORTED_LOCATION ${WEBSOCKETPP_DIR}/lib/libwebsocketpp${STATIC_LIBRARY_SUFFIX})
endif()

add_library(websocketpp_debug STATIC IMPORTED)
add_library(websocketpp_release STATIC IMPORTED)

if(WIN32)
  set_property(TARGET websocketpp_debug PROPERTY IMPORTED_LOCATION ${WEBSOCKETPP_DIR}/lib/websocketppd.lib)
  set_property(TARGET websocketpp_release PROPERTY IMPORTED_LOCATION ${WEBSOCKETPP_DIR}/lib/websocketpp.lib)
else() # Linux and Mac
  set_property(TARGET websocketpp_debug PROPERTY IMPORTED_LOCATION ${WEBSOCKETPP_DIR}/lib/libwebsocketpp${STATIC_LIBRARY_SUFFIX})
  set_property(TARGET websocketpp_release PROPERTY IMPORTED_LOCATION ${WEBSOCKETPP_DIR}/lib/libwebsocketpp${STATIC_LIBRARY_SUFFIX})
endif()

add_definitions(-USEAUTONET)
