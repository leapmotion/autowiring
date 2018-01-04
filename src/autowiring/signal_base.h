// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once

namespace autowiring {
  struct registration_t;

  struct signal_base {
    virtual ~signal_base(void) {}

    /// <returns>
    /// True if signal handlers are presently being invoked
    /// </returns>
    virtual bool is_executing(void) const = 0;

    /// <summary>
    /// Removes the signal node identified on the rhs without requiring full type information
    /// </summary>
    /// <returns>
    /// True if the registration could be removed, false if the request could not be completed
    /// before the function returned control.
    /// </returns>
    /// <remarks>
    /// Regardless of the return code, this routine guarantees that the specified registration will
    /// _eventually_ be removed from the signal, provided that the application is free from
    /// deadlocks and the argument is valid.
    ///
    /// This operation invalidates the specified unique pointer.  If the passed unique pointer is
    /// already nullptr, this operation has no effect.
    /// </remarks>
    virtual bool operator-=(registration_t& rhs) = 0;
  };
}
