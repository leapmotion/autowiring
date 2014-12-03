// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include <sstream>
#include TYPE_INDEX_HEADER

namespace autowiring {
  std::string ExtractKeyUnix(std::stringstream& ss);
  std::string ExtractKeyWin(std::stringstream& ss);
  std::string ExtractKey(const std::type_info& ti);
}
