// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <string>
#include TYPE_INDEX_HEADER

namespace autowiring {
  std::string ExtractKey(const std::string& ss);
  std::string ExtractKey(const std::type_info& ti);
}
