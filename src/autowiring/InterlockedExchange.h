// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once

/// <summary>
/// Stores the passed value in the specified space, and returns the value that was previously located there
/// </summary>
void* exchange_acquire(void** dest, void* exchange);
void* exchange_release(void** dest, void* exchange);

/// <summary>
/// Interlocked 32-bit add routine
/// </summary>
long interlocked_add(long* dest, long addBy);

/// <summary>
/// Performs an interlocked compare exchange with the passed field
/// </summary>
/// <returns>The value of *dest prior to the swap</returns>
void* compare_exchange(void** dest, void* exchange, const void* comparand);

template<class T>
T* compare_exchange(const T*const* dest, T* exchange, const T* comparand) {
  return (T*)compare_exchange((void**) dest, exchange, comparand);
}
