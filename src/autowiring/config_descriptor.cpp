// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "config_descriptor.h"

autowiring::config_descriptor::t_mpType autowiring::config_descriptor::MakeFields(const std::initializer_list<config_field>& fields) {
  t_mpType retVal;
  for (auto& field : fields)
    retVal[field.name] = field;
  return retVal;
}
