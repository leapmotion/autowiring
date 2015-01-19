// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_arg.h"
#include "AutoPacket.h"
#include "Decompose.h"
#include <assert.h>

class Deferred;

// The type of the call centralizer
typedef void(*t_extractedCall)(const AnySharedPointer& obj, AutoPacket&);

template<class MemFn>
struct CallExtractor;

/// <summary>
/// Specialization for nonmember function calls
/// </summary>
template<class RetType, class... Args>
struct CallExtractor<RetType (*)(Args...)>:
  Decompose<RetType(*)(Args...)>
{
  static const bool has_outputs = is_any<auto_arg<Args>::is_output...>::value;
  static const bool deferred = false;
  static const bool stateless = true;

  /// <summary>
  /// Binder struct, lets us refer to an instance of Call by type
  /// </summary>
  static void Call(const AnySharedPointer& obj, AutoPacket& autoPacket) {
    const void* pfn = obj->ptr();

    // This is the true type of the input, it's the fnptr itself, not a function object
    typedef RetType(*t_pfn)(Args...);

    // Handoff
    ((t_pfn)pfn)(
      typename auto_arg<Args>::type(autoPacket)...
    );
  }
};

/// <summary>
/// Specialization for member function AutoFilter functions
/// </summary>
template<class T, class... Args>
struct CallExtractor<void (T::*)(Args...)>:
  Decompose<void (T::*)(Args...)>
{
  static const bool has_outputs = is_any<auto_arg<Args>::is_output...>::value;
  static const bool stateless = false;
  static const bool deferred = false;

  /// <summary>
  /// Binder struct, lets us refer to an instance of Call by type
  /// </summary>
  template<void(T::*memFn)(Args...)>
  static void Call(const AnySharedPointer& obj, AutoPacket& autoPacket) {
    const void* pObj = obj->ptr();

    // This exception type indicates that an attempt was made to construct an AutoFilterDescriptor with an
    // AnySharedPointer which was not the type of its own member function.  Be sure to cast the AnySharedPointer
    // to the correct foundation type before attempting to construct an AutoFilterDescriptor.
    assert(typeid(auto_id<T>) == obj->type());

    // Handoff
    (((T*) pObj)->*memFn)(
      typename auto_arg<Args>::type(autoPacket)...
    );
  }
};

/// <summary>
/// Specialization for stateless member function AutoFilter routines
/// </summary>
template<class T, class... Args>
struct CallExtractor<void (T::*)(Args...) const> :
  Decompose<void (T::*)(Args...)>
{
  static const bool has_outputs = is_any<auto_arg<Args>::is_output...>::value;
  static const bool stateless = true;
  static const bool deferred = false;
  
  template<void(T::*memFn)(Args...) const>
  static void Call(const AnySharedPointer& obj, AutoPacket& autoPacket) {
    const void* pObj = obj->ptr();

    // Handoff
    (((const T*) pObj)->*memFn)(
      typename auto_arg<Args>::type(autoPacket)...
    );
  }
};

/// <summary>
/// Specialization for deferred member function AutoFilter routines
/// </summary>
template<class T, class... Args>
struct CallExtractor<Deferred (T::*)(Args...)>:
  Decompose<void (T::*)(Args...)>
{
  static const bool has_outputs = is_any<auto_arg<Args>::is_output...>::value;
  static const bool stateless = false;
  static const bool deferred = true;

  template<Deferred(T::*memFn)(Args...)>
  static void Call(const AnySharedPointer& obj, AutoPacket& autoPacket) {
    const void* pObj = obj->ptr();

    // Obtain a shared pointer of the AutoPacket in order to ensure the packet
    // is not destroyed when we pend this lambda to the destination object's
    // dispatch queue.
    auto pAutoPacket = autoPacket.shared_from_this();

    // Pend the call to this object's dispatch queue:
    *(T*) pObj += [pObj, pAutoPacket] {
      (((T*) pObj)->*memFn)(
        typename auto_arg<Args>::type(*pAutoPacket)...
      );
    };
  }
};