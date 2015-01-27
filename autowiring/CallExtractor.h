// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_arg.h"
#include "auto_tuple.h"
#include "AutoPacket.h"
#include "CurrentContextPusher.h"
#include "Decompose.h"
#include "index_tuple.h"
#include <cassert>

class Deferred;

// The type of the call centralizer
typedef void(*t_extractedCall)(const AnySharedPointer& obj, AutoPacket&);

template<class MemFn, class Index = typename make_index_tuple<Decompose<MemFn>::N>::type>
struct CallExtractor;

namespace autowiring {
  template<class... Args>
  void noop(Args...) {}
}

template<class... Args>
struct CallExtractorSetup
{
  CallExtractorSetup(AutoPacket& packet):
    packet(packet),
    pshr(packet.GetContext()),
    args(
      (auto_arg<Args>::arg(packet))...
    )
  {}

  AutoPacket& packet;
  CurrentContextPusher pshr;
  autowiring::tuple<typename auto_arg<Args>::type...> args;

  template<int N>
  typename std::enable_if<
    auto_arg<typename autowiring::nth_type<N, Args...>::type>::is_output,
    bool
  >::type Commit(bool) {
    packet.Decorate(autowiring::get<N>(args));
    return true;
  }

  template<int N>
  bool Commit(...) { return false; }
};

/// <summary>
/// Specialization for nonmember function calls
/// </summary>
template<class RetType, class... Args, int... N>
struct CallExtractor<RetType (*)(Args...), index_tuple<N...>>:
  Decompose<RetType(*)(Args...)>
{
  static const bool has_outputs = is_any<auto_arg<Args>::is_output...>::value;
  static const bool deferred = false;
  static const bool stateless = true;

  // This is the true type of the input, it's the fnptr itself, not a function object
  typedef RetType(*t_pfn)(Args...);

  /// <summary>
  /// Binder struct, lets us refer to an instance of Call by type
  /// </summary>
  static void Call(const AnySharedPointer& obj, AutoPacket& packet) {
    const void* pfn = obj->ptr();
    
    // Setup, handoff, commit
    CallExtractorSetup<Args...> extractor(packet);
    ((t_pfn)pfn)(
      static_cast<typename auto_arg<Args>::arg_type>(autowiring::get<N>(extractor.args))...
    );
    autowiring::noop(extractor.template Commit<N>(false)...);
  }
};

/// <summary>
/// Specialization for member function AutoFilter functions
/// </summary>
template<class T, class... Args, int... N>
struct CallExtractor<void (T::*)(Args...), index_tuple<N...>> :
  Decompose<void (T::*)(Args...)>
{
  static const bool has_outputs = is_any<auto_arg<Args>::is_output...>::value;
  static const bool stateless = false;
  static const bool deferred = false;

  /// <summary>
  /// Binder struct, lets us refer to an instance of Call by type
  /// </summary>
  template<void(T::*memFn)(Args...)>
  static void Call(const AnySharedPointer& obj, AutoPacket& packet) {
    const void* pObj = obj->ptr();

    // This exception type indicates that an attempt was made to construct an AutoFilterDescriptor with an
    // AnySharedPointer which was not the type of its own member function.  Be sure to cast the AnySharedPointer
    // to the correct foundation type before attempting to construct an AutoFilterDescriptor.
    assert(typeid(auto_id<T>) == obj->type());

    // Extract, call, commit
    CallExtractorSetup<Args...> extractor(packet);
    (((T*) pObj)->*memFn)(
      static_cast<typename auto_arg<Args>::arg_type>(autowiring::get<N>(extractor.args))...
    );
    autowiring::noop(extractor.template Commit<N>(false)...);
  }
};

/// <summary>
/// Specialization for stateless member function AutoFilter routines
/// </summary>
template<class T, class... Args, int... N>
struct CallExtractor<void (T::*)(Args...) const, index_tuple<N...>> :
  Decompose<void (T::*)(Args...)>
{
  static const bool has_outputs = is_any<auto_arg<Args>::is_output...>::value;
  static const bool stateless = true;
  static const bool deferred = false;
  
  template<void(T::*memFn)(Args...) const>
  static void Call(const AnySharedPointer& obj, AutoPacket& packet) {
    const void* pObj = obj->ptr();

    // Extract, call, commit
    CallExtractorSetup<Args...> extractor(packet);
    (((const T*) pObj)->*memFn)(
      static_cast<typename auto_arg<Args>::arg_type>(autowiring::get<N>(extractor.args))...
    );
    autowiring::noop(extractor.template Commit<N>(false)...);
  }
};

/// <summary>
/// Specialization for deferred member function AutoFilter routines
/// </summary>
template<class T, class... Args, int... N>
struct CallExtractor<Deferred(T::*)(Args...), index_tuple<N...>> :
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

      // Extract, call, commit
      CallExtractorSetup<Args...> extractor(*pAutoPacket);
      (((T*) pObj)->*memFn)(
        static_cast<typename auto_arg<Args>::arg_type>(autowiring::get<N>(extractor.args))...
      );
      autowiring::noop(extractor.template Commit<N>(false)...);
    };
  }
};
