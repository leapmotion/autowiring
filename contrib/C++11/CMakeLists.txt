set(
  Cpp11_SRCS
  boost_atomic.h
  boost_exception_ptr.h
  boost_functional.h
  boost_future.h
  boost_rvalue.h
  boost_shared_ptr.h
  boost_tuple.h
  boost_type_traits.h
  cpp11.h
  make_unique.h
  memory.h
  memory_nostl11.h
  stdafx.cpp
  stdafx.h
  type_index.h
  unique_ptr.h
)

if(BUILD_WINDOWS)
  # warngs: stdafx.cpp.o has no symbols
  # test-compile on Windows, although not necessary there either
  add_library(Cpp11 STATIC ${Cpp11_SRCS})
  set_property(TARGET Cpp11 PROPERTY FOLDER "Utility")
endif()
