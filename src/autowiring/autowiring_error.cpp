// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "autowiring_error.h"
#include "demangle.h"
#include <sstream>

using namespace autowiring;

autowiring_error::autowiring_error(const std::string& what):
  m_what(what)
{}

autowiring_error::~autowiring_error(void){}

void autowiring::ThrowFailedTypeParseException(const std::string& str, const std::type_info& ti) {
  std::stringstream msg;
  msg << "Failed to parse '" << str << "' as type '" << autowiring::demangle(ti) << "'";
  throw autowiring_error(msg.str());
}
