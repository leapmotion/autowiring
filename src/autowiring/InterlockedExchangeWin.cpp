// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "InterlockedExchange.h"
#include <windows.h>

void* autowiring::exchange_acquire(void** dest, void* exchange) {
  return InterlockedExchangePointer(dest, exchange);
}

void* autowiring::exchange_release(void** dest, void* exchange) {
  return InterlockedExchangePointer(dest, exchange);
}

long autowiring::interlocked_add(long* dest, long addBy) {
  return InterlockedExchangeAdd(dest, addBy);
}

void* autowiring::compare_exchange(void** dest, void* exchange, const void* comparand) {
  return InterlockedCompareExchangePointer(
    dest,
    exchange,
    (void*)comparand
  );
}
