// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "demangle.h"
#include "AnySharedPointer.h"
#include <sstream>
#include <typeindex>

#if __GNUG__ // Mac and linux
#include <cxxabi.h>
#include <cstdlib>

static std::string demangle_name(const char* name) {
  int status;
  std::unique_ptr<char, void(*)(void*)> res{
    abi::__cxa_demangle(name, nullptr, nullptr, &status),
    std::free
  };

  if(status != 0)
    return std::string();

  return std::string(res.get());
}

#else // Windows

static std::string demangle_name(const char* name) {
  if (strchr(name, '`'))
    return std::string{};

  // Number of left <'s we have seen
  int nLeft = 0;

  // "class" or "struct" prefixes should be eliminated
  std::ostringstream os;
  while(*name) {
    if (strncmp(name, "class ", 6) == 0) {
      name += 6;
      continue;
    }

    if (strncmp(name, "struct ", 7) == 0) {
      name += 7;
      continue;
    }

    os << *name;
    if(*name == ',')
      // Put a space after all commas, required for proper template parsing
      os << ' ';

    name++;
  }
  return os.str();
}

#endif

std::string autowiring::demangle(const std::type_info& ti) {
  return demangle_name(ti.name());
}

std::string autowiring::demangle(const std::type_info* ti) {
  return demangle_name(ti->name());
}

std::string autowiring::demangle(const std::type_index& ti) {
  return demangle_name(ti.name());
}

std::string autowiring::demangle(const std::type_index* ti) {
  return demangle_name(ti->name());
}

std::string autowiring::demangle(auto_id id) {
  return
    id.block->ti ?
    autowiring::demangle(id.block->ti) :
    "";
}

std::string autowiring::demangle(const AnySharedPointer& ptr) {
  return autowiring::demangle(ptr.type());
}

