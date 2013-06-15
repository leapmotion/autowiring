#include "stdafx.h"
#include "InterlockedExchange.h"

void* exchange_acquire(void*volatile* dest, void* exchange) {
  return __sync_lock_test_and_set((void**)dest, exchange);
}

void* exchange_release(void*volatile* dest, void* exchange) {
  return __sync_lock_test_and_set((void**)dest, exchange);
}

void* compare_exchange(void*volatile* dest, void* exchange, const void* comparand) {
  return __sync_val_compare_and_swap((void**)dest, (void*)comparand, exchange);
}
