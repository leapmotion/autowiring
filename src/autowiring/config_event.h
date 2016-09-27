// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include "config_descriptor.h"

namespace autowiring {
struct config_descriptor;
struct config_field;
struct metadata_base;

struct config_event {
  config_event(void) = default;
  config_event(std::shared_ptr<void> pObj, const config_descriptor* desc, const config_field* field_desc, const metadata_base* metadata) :
    pObj(pObj),
    desc(desc),
    field_desc(field_desc),
    metadata(metadata)
  {}

  // A pointer to the enclosing object on which the event is being asserted
  std::shared_ptr<void> pObj;

  // A pointer to the field itself
  void* field(void) const {
    return static_cast<char*>(pObj.get()) + field_desc->offset;
  }

  // Metadata and descriptor pointers
  const config_descriptor* desc;
  const config_field* field_desc;
  const metadata_base* metadata;
};
}
