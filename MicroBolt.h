#pragma once
#include "Bolt.h"
#include "CreationRules.h"
#include "Decompose.h"
#include "is_any.h"
#include <vector>

/// <summary>
/// Causes the class inheriting from this definition to be bolted to the specified contexts
/// </summary>
template<typename... Sigils>
struct Boltable {};

/// <summary>
/// A default bolt type which will insert the specified type into a matching context
/// </summary>
template<typename T, typename... Sigils>
struct MicroBolt:
  public Bolt<Sigils...>
{
public:
  static_assert(!is_any<void, Sigils...>::value, "Can't use 'void' as a sigil type");
  void ContextCreated(void) override;
};

template<typename T>
struct MicroBolt<T, void> {};

#include "CoreContext.h"

template<typename T, typename... Sigils>
void MicroBolt<T, Sigils...>::ContextCreated(void) {
  CoreContext::InjectCurrent<T>();
}
