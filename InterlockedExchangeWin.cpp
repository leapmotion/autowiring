#include "stdafx.h"
#include "InterlockedExchange.h"
#include <windows.h>

void* exchange_acquire(void*volatile* dest, void* exchange) {
  return InterlockedExchangePointer(dest, exchange);
}

void* exchange_release(void*volatile* dest, void* exchange) {
  return InterlockedExchangePointer(dest, exchange);
}

void* compare_exchange(void*volatile* dest, void* exchange, const void* comparand) {
  return InterlockedCompareExchangePointer(
    dest,
    exchange,
    (void*)comparand
  );
}