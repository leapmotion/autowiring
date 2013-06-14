#include "stdafx.h"
#include "InterlockedExchange.h"

void* exchange_acquire(void*volatile* dest, void* exchange) {
  __asm__ ("xchg %0, %1" : "+q" (exchange), "+m" (*dest));
  __asm__ __volatile__ ("" ::: "memory");
  return exchange;
}

void* exchange_release(void*volatile* dest, void* exchange) {
  __asm__ __volatile__ ("" ::: "memory");
  __asm__ ("xchg %0, %1" : "+q" (exchange), "+m" (*dest));
  return exchange;
}

void* compare_exchange(void*volatile* dest, void* exchange, const void* comparand) {
  void* previous = const_cast<void*>(comparand);
  __asm__ __volatile__ ("" ::: "memory");
  __asm__ ("lock ; cmpxchg %2, %1" : "+a" (previous), "+m" (*dest) : "q" (exchange));
  if (previous == comparand) {
    __asm__ __volatile__ ("" ::: "memory");
  }
  return previous;
}
