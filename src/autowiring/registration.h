// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include "signal_base.h"

namespace autowiring {
  struct registration_t {
    registration_t(void) = default;

    registration_t(signal_base* owner, void* pobj) :
      owner(owner),
      pobj(pobj)
    {}

    registration_t(registration_t&& rhs) :
      owner(rhs.owner),
      pobj(rhs.pobj)
    {
      rhs.pobj = nullptr;
    }

    registration_t(const registration_t& rhs) = delete;

    signal_base* owner = nullptr;
    void* pobj = nullptr;

    bool operator==(const registration_t& rhs) const { return pobj == rhs.pobj; }

    void operator=(registration_t&& rhs) {
      owner = rhs.owner;
      pobj = rhs.pobj;
      rhs.pobj = nullptr;
    }

    void clear(void) {
      if(pobj)
        *owner -= *this;
    }

    operator bool(void) const { return pobj != nullptr; }
  };
}
