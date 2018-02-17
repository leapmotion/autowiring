// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once
#include "Bolt.h"
#include "CoreContext.h"
#include "ContextEnumerator.h"

/// <summary>
/// Causes the class inheriting from this definition to be bolted to the specified contexts
/// </summary>
template<typename... Sigils>
struct InjectBolt {};

/// <summary>
/// A default bolt type which will insert the specified type into a matching context
/// </summary>
template<typename T, typename... Sigils>
struct MicroBolt:
  public Bolt<Sigils...>
{
private:
  template<class W>
  static bool LoopInject(const std::shared_ptr<CoreContext>& ctxt) {
    for(auto context : ContextEnumeratorT<W>(ctxt))
      // Inject<W>() is idempotent, that makes this behavior safe
      context->template Inject<T>();
    return true;
  }

public:
  MicroBolt(void) {
    // NOTE: Injection of T into all matching contexts may result in
    // multiple calls to Inject<T>() if a matching context
    // is created during traversal.
    const auto ctxt = CoreContext::CurrentContext();
    autowiring::noop(LoopInject<Sigils>(ctxt)...);
  }
  void ContextCreated(void) override;
};

template<typename T, typename... Sigils>
void MicroBolt<T, Sigils...>::ContextCreated(void) {
  CoreContext::InjectCurrent<T>();
}
