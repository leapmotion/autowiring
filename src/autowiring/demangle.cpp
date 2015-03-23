// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "demangle.h"
#include "AnySharedPointer.h"

#if __GNUG__ // Mac and linux
#include <cxxabi.h>
#include <cstdlib>

std::string autowiring::demangle(const std::type_info& ti, bool* success) {
  int status;
  std::unique_ptr<char, void(*)(void*)> res{
    abi::__cxa_demangle(ti.name(), nullptr, nullptr, &status),
    std::free
  };
  if(success != nullptr)
    *success = status == 0;
  
  return std::string(status == 0 ? res.get() : ti.name());
}

#else // Windows

std::string autowiring::demangle(const std::type_info& ti, bool* success) {
  std::string demangled(ti.name());
  
  if(success != nullptr)
    *success = demangled.find("`") == std::string::npos;
  
  return std::string(ti.name());
}

#endif

std::string autowiring::demangle(const AnySharedPointer& ptr, bool* success) {
  return autowiring::demangle(ptr->type(), success);
}

std::string autowiring::demangle(const std::type_info* ti, bool* success) {
  return autowiring::demangle(*ti, success);
}

