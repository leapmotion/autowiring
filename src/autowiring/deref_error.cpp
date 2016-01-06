// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "deref_error.h"
#include "demangle.h"
#include <sstream>

static std::string GenerateExceptionText(const char* type, const std::type_info& ti) {
  std::stringstream ss;
  ss << "Attempted to dereference a null " << type << "<" << autowiring::demangle(ti) << ">";
  return ss.str();
}

std::string autowiring::GenerateExceptionTextAWF(const std::type_info& ti) {
  return GenerateExceptionText("AutowiredFast", ti);
}

std::string autowiring::GenerateExceptionTextAW(const std::type_info& ti) {
  return GenerateExceptionText("Autowired", ti);
}
