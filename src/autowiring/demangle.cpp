// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "demangle.h"
#include "AnySharedPointer.h"

#if __GNUG__ // Mac and linux
#include <cxxabi.h>
#include <cstdlib>

std::string autowiring::demangle(const std::type_info& ti) {
  int status;
  std::unique_ptr<char, void(*)(void*)> res{
    abi::__cxa_demangle(ti.name(), nullptr, nullptr, &status),
    std::free
  };

  if(status != 0)
    return std::string();
  
  return std::string(res.get());
}

#else // Windows

std::string autowiring::demangle(const std::type_info& ti) {
  std::string demangled(ti.name());
  
  if (demangled.find("`") != std::string::npos)
    return std::string();
  
  return demangled;
}

#endif

std::string autowiring::demangle(const AnySharedPointer& ptr) {
  return autowiring::demangle(ptr->type());
}

std::string autowiring::demangle(const std::type_info* ti) {
  return autowiring::demangle(*ti);
}

