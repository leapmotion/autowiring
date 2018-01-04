// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once
#include "autowiring_error.h"

/// \internal
/// <summary>
/// Thrown when a dispatch operation was aborted
/// </summary>
class dispatch_aborted_exception:
  public autowiring_error
{
public:
  dispatch_aborted_exception(void);
  dispatch_aborted_exception(const std::string& what);
  virtual ~dispatch_aborted_exception(void);
};
