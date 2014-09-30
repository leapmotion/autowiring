// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "DataFlow.h"
#include "Decompose.h"
#include "AutoPacket.h"
#include "auto_arg.h"

class Deferred;

/// <summary>
/// Specialization for immediate mode cases
/// </summary>
template<class MemFn>
struct CallExtractor;

template<class RetType, class... Args>
struct CallExtractor<RetType (*)(Args...)>:
  Decompose<RetType(*)(Args...)>
{
  static const bool deferred = false;

  /// <summary>
  /// Binder struct, lets us refer to an instance of Call by type
  /// </summary>
  static void Call(void* pfn, AutoPacket& autoPacket, const autowiring::DataFill& satisfaction) {
    // This is the true type of the input, it's the fnptr itself, not a function object
    typedef RetType(*t_pfn)(Args...);

    // Handoff
    ((t_pfn)pfn)(
      auto_arg<Args>(autoPacket.shared_from_this(), *satisfaction.source<typename auto_arg<Args>::base_type>())...
    );
  }
};

template<class T, class... Args>
struct CallExtractor<void (T::*)(Args...)>:
  Decompose<void (T::*)(Args...)>
{
  static const bool deferred = false;
  static const size_t N = sizeof...(Args);

  /// <summary>
  /// Binder struct, lets us refer to an instance of Call by type
  /// </summary>
  template<void(T::*memFn)(Args...)>
  static void Call(void* pObj, AutoPacket& autoPacket, const autowiring::DataFill& satisfaction) {
    // Handoff
    (((T*) pObj)->*memFn)(
      auto_arg<Args>(autoPacket.shared_from_this(), *satisfaction.source<typename auto_arg<Args>::base_type>())...
    );
  }
};

/// <summary>
/// Specialization for deferred cases
/// </summary>
template<class T, class... Args>
struct CallExtractor<Deferred (T::*)(Args...)>:
  Decompose<void (T::*)(Args...)>
{
  static const bool deferred = true;
  static const size_t N = sizeof...(Args);

  template<Deferred(T::*memFn)(Args...)>
  static void Call(void* pObj, AutoPacket& autoPacket, const autowiring::DataFill& satisfaction) {
    // Obtain a shared pointer of the AutoPacket in order to ensure the packet
    // stays resident when we pend this lambda to the destination object's
    // dispatch queue.
    auto pAutoPacket = autoPacket.shared_from_this();

    // Pend the call to this object's dispatch queue:
    // WARNING: The autowiring::DataFill information will be referenced,
    // since it should be from a SatCounter associated to autoPacket,
    // and will therefore have the same lifecycle as the AutoPacket.
    *(T*) pObj += [pObj, pAutoPacket, &satisfaction] {
      (((T*) pObj)->*memFn)(
        auto_arg<Args>(pAutoPacket, *satisfaction.source<typename auto_arg<Args>::base_type>())...
      );
    };
  }
};