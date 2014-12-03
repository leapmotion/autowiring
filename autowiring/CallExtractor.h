// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_arg.h"
#include "AutoPacket.h"
#include "Decompose.h"

class Deferred;

// The type of the call centralizer
typedef void(*t_extractedCall)(const AnySharedPointer& obj, AutoPacket&);

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
  static void Call(const AnySharedPointer& obj, AutoPacket& autoPacket) {
    const void* pfn = obj->ptr();

    // This is the true type of the input, it's the fnptr itself, not a function object
    typedef RetType(*t_pfn)(Args...);

    // Handoff
    ((t_pfn)pfn)(
      auto_arg<Args>(autoPacket.shared_from_this())...
    );
  }
};

template<class T, class... Args>
struct CallExtractor<void (T::*)(Args...)>:
  Decompose<void (T::*)(Args...)>
{
  static const bool stateless = false;
  static const bool deferred = false;
  static const size_t N = sizeof...(Args);

  /// <summary>
  /// Binder struct, lets us refer to an instance of Call by type
  /// </summary>
  template<void(T::*memFn)(Args...)>
  static void Call(const AnySharedPointer& obj, AutoPacket& autoPacket) {
    const void* pObj = obj->ptr();

    // This exception type indicates that an attempt was made to construct an AutoFilterDescriptor with an
    // AnySharedPointer which was not the type of its own member function.  Be sure to cast the AnySharedPointer
    // to the correct foundation type before attempting to construct an AutoFilterDescriptor.
    assert(typeid(T) == obj->type());

    // Handoff
    (((T*) pObj)->*memFn)(
      auto_arg<Args>(autoPacket.shared_from_this())...
    );
  }
};

/// <summary>
/// Specialization for stateless AutoFilter routines
/// </summary>
template<class T, class... Args>
struct CallExtractor<void (T::*)(Args...) const> :
  Decompose<void (T::*)(Args...)>
{
  static const bool stateless = true;
  static const bool deferred = false;
  static const size_t N = sizeof...(Args);
  
  template<void(T::*memFn)(Args...) const>
  static void Call(const AnySharedPointer& obj, AutoPacket& autoPacket) {
    const void* pObj = obj->ptr();

    // Handoff
    (((const T*) pObj)->*memFn)(
      auto_arg<Args>(autoPacket.shared_from_this())...
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
  static const bool stateless = false;
  static const bool deferred = true;
  static const size_t N = sizeof...(Args);

  template<Deferred(T::*memFn)(Args...)>
  static void Call(const AnySharedPointer& obj, AutoPacket& autoPacket) {
    const void* pObj = obj->ptr();

    // Obtain a shared pointer of the AutoPacket in order to ensure the packet
    // stays resident when we pend this lambda to the destination object's
    // dispatch queue.
    auto pAutoPacket = autoPacket.shared_from_this();

    // Pend the call to this object's dispatch queue:
    // WARNING: The autowiring::DataFill information will be referenced,
    // since it should be from a SatCounter associated to autoPacket,
    // and will therefore have the same lifecycle as the AutoPacket.
    *(T*) pObj += [pObj, pAutoPacket] {
      (((T*) pObj)->*memFn)(
        auto_arg<Args>(pAutoPacket)...
      );
    };
  }
};