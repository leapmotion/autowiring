// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include TYPE_INDEX_HEADER
#include <string>

#if __GNUG__ // Mac and linux
#include <cxxabi.h>
#include <cstdlib>
#include MEMORY_HEADER
#endif

//
// Demangle type names on mac and linux.
// Just returns type_info.name() on windows
//
namespace autowiring {
  std::string demangle(const std::type_info& ti);
  std::string demangle(const std::type_info* ti);

  template<typename T>
  std::string demangle(const T&) {
    return demangle(typeid(T));
  }
}//namespace autowiring
