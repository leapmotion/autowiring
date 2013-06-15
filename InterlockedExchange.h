#ifndef _INTERLOCKED_EXCHANGE_H
#define _INTERLOCKED_EXCHANGE_H

/// <summary>
/// Stores the passed value in the specified space, and returns the value that was previously located there
/// </summary>
void* exchange_acquire(void*volatile* dest, void* exchange);
void* exchange_release(void*volatile* dest, void* exchange);

/// <summary>
/// Performs an interlocked compare exchange with the passed field
/// </summary>
/// <returns>The value of *dest prior to the swap</returns>
void* compare_exchange(void*volatile* dest, void* exchange, const void* comparand);

#endif