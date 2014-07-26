// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include MEMORY_HEADER

/// <summary>
/// Marks an input to an AutoFilter whose memory payload should be pooled
/// </summary>
/// <remarks>
/// </remarks>
template<class T>
class auto_pooled:
  public std::shared_ptr<T>
{
public:
};
