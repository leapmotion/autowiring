# We need type traits on this platform for all of our enable_if stuff
if(APPLE AND NOT USE_LIBCXX)
  message("Cannot build AutoNet without USE_LIBCXX due to extensive use of C++11 facilities")
  return()
endif()

add_library(json11 STATIC json11.cpp json11.hpp)

# Hide contrib/ when we are embedded
if(AUTOWIRING_IS_EMBEDDED)
  set_property(TARGET json11 PROPERTY FOLDER "Autowiring/contrib")
else()
  set_property(TARGET json11 PROPERTY FOLDER "contrib")
endif()
