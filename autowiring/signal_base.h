// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once

namespace autowiring {
  struct registration_t;

  struct signal_base {
    virtual ~signal_base(void) {}

    /// <summary>
    /// Removes the signal node identified on the rhs without requiring full type information
    /// </summary>
    /// <remarks>
    /// This operation invalidates the specified unique pointer.  If the passed unique pointer is
    /// already nullptr, this operation has no effect.
    /// </remarks>
    virtual void operator-=(registration_t& rhs) = 0;
  };
}
