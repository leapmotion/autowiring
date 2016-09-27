// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include "is_any.h"
#include "BoltBase.h"

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
  const auto_id* GetContextSigils(void) const override {
    static const auto_id s_types[] = {
      auto_id_t<Sigil>{}...,
      auto_id_t<void>{}
    };
    return s_types;
  }

  bool Matches(auto_id id) const override {
    for (auto cur : { auto_id{auto_id_t<Sigil>{}}... })
      if(cur == id)
        return true;
    return false;
  }

  static_assert(!autowiring::is_any_same<void, Sigil...>::value, "Can't use 'void' as a sigil type");
};

template<>
class Bolt<>:
  public BoltBase
{
public:
  const auto_id* GetContextSigils(void) const override {
    static const auto_id s_types[] = { {} };
    return s_types;
  }

  virtual bool Matches(auto_id id) const override { return true; }
};
