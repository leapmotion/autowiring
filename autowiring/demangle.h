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
#if __GNUG__ // Mac and linux
  
  static inline std::string demangle(const std::type_info& ti) {
    int status;
    std::unique_ptr<char, void(*)(void*)> res{
      abi::__cxa_demangle(ti.name(), nullptr, nullptr, &status),
      std::free
    };
    return std::string(status == 0 ? res.get() : ti.name());
  }
  
#else // Windows
  
  static inline std::string demangle(const std::type_info& ti) {
    return std::string(ti.name());
  }
  
#endif

  static inline std::string demangle(const std::type_info* ti) {
    return demangle(*ti);
  }

  template<typename T>
  static inline std::string demangle(const T&) {
    return demangle(typeid(T));
  }
}//namespace autowiring
