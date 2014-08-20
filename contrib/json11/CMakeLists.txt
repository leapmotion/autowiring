# We need type traits on this platform for all of our enable_if stuff
if(APPLE AND NOT USE_LIBCXX)
  message("Cannot build AutoNet without USE_LIBCXX due to extensive use of C++11 facilities")
  return()
endif()

add_library(json11 STATIC json11.cpp json11.hpp)
set_property(TARGET json11 PROPERTY FOLDER "contrib")
