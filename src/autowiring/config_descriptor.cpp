// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "config_descriptor.h"

using namespace autowiring;

config_descriptor::t_mpType autowiring::config_descriptor::MakeFields(const std::initializer_list<config_field>& fields) {
  t_mpType retVal;
  for (const config_field & field : fields)
    retVal[field.name] = field;
  return retVal;
}

config_descriptor::config_descriptor(void) {}

const config_field& config_descriptor::get(size_t offset) const {
  for (const auto& entry : fields)
    if (entry.second.offset == offset)
      return entry.second;
  throw std::range_error("Attempted to find a configuration field at an invalid offset");
}
