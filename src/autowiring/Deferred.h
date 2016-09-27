// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once

class DispatchQueue;

/// <summary>
/// Marker return type for deferred calls
/// </summary>
/// <remarks>
/// A marker type for AutoFilter functions in CoreThreads. If an AutoFilter returns this type,
/// the function will be run in its own thread rather than the thread that decorated the type
/// </remarks>
class Deferred {
public:
  Deferred(DispatchQueue* pQueue) {}
};
