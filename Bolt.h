#pragma once

#include "BoltBase.h"
#include "Decompose.h"
#include TYPE_INDEX_HEADER

/// <summary>
/// </summary>
/// <param name="Sigil">The sigils of the contexts whose creation we want to listen for</param>
/// <remarks>
/// A bolt is a way to augment the creation of a new context in a declarative, by-name manner.
/// By inheriting from Bolt, the object will recieve ContextCreated() calls.
/// To create a class that will have a new instance inserted into each instance of a context
/// with a given sigil, use Boltable.
/// </remarks>

template<typename... Sigil>
class Bolt:
  public BoltBase
{
public:
  const t_TypeInfoVector GetContextSigils(void) override {
    static const std::type_info* sc_types[] = {
      &typeid(Sigil)...,
      nullptr
    };
    return sc_types;
  }

  static_assert(!is_any_same<void, Sigil...>::value, "Can't use 'void' as a sigil type");
};

template<>
class Bolt<>:
  public BoltBase
{
public:
  const t_TypeInfoVector GetContextSigils(void) override {
    static const std::type_info* sc_types[] = {nullptr};
    return sc_types;
  }
};
