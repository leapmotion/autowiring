// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "InterlockedExchange.h"

void* autowiring::exchange_acquire(void** dest, void* exchange) {
  return __sync_lock_test_and_set((void**)dest, exchange);
}

void* autowiring::exchange_release(void** dest, void* exchange) {
  return __sync_lock_test_and_set((void**)dest, exchange);
}

void* autowiring::compare_exchange(void** dest, void* exchange, const void* comparand) {
  return __sync_val_compare_and_swap((void**)dest, (void*)comparand, exchange);
}
