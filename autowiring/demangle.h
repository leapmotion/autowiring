// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <string>
#include <typeinfo>

struct AnySharedPointer;

//
// Demangle type names on mac and linux.
// Just returns type_info.name() on windows
//
namespace autowiring {
  std::string demangle(const std::type_info& ti);
  std::string demangle(const std::type_info* ti);

  std::string demangle(const AnySharedPointer& ptr);

  /// Returns a human-readable std::string describing the type.
  template<typename T>
  std::string demangle(const T&) {
    return demangle(typeid(T));
  }

}//namespace autowiring
