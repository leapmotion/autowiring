// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_arg.h"
#include "auto_in.h"
#include "AutoPacket.h"
#include "CurrentContextPusher.h"
#include "Decompose.h"
#include "noop.h"
#include TUPLE_HEADER

class Deferred;

namespace autowiring {

// The type of the call centralizer
typedef void(*t_extractedCall)(const void* obj, AutoPacket&);


  template<int N, class... Args>
  struct nth_type;

  template<class Head, class... Tail>
  struct nth_type<0, Head, Tail...> {
    typedef Head type;
  };

  template<int N, class Head, class... Tail>
  struct nth_type<N, Head, Tail...> :
    nth_type<N - 1, Tail...>
  {};

/// <summary>
/// An argument pack that holds all of the inputs and outputs to an AutoFilter during its invocation
/// </summary>
/// <param name="shared_outputs">Holds true if output types should be declared as shared pointers, false otherwise</param>
template<class... Args>
struct CESetup {
  CESetup(AutoPacket& packet) :
    packet(packet),
    pshr(packet.GetContext()),
    args(auto_arg<Args>::arg(packet)...)
  {}

  AutoPacket& packet;
  CurrentContextPusher pshr;
  std::tuple<typename auto_arg<Args>::type...> args;

  template<int N>
  typename std::enable_if<
    auto_arg<typename autowiring::nth_type<N, Args...>::type>::is_output,
    bool
  >::type Commit(bool) {
    auto_arg<typename autowiring::nth_type<N, Args...>::type>::Commit(packet, std::get<N>(args));
    return true;
  }

  template<int N>
  bool Commit(...) { return false; }
};

// Slightly more efficient no-argument specialization
template<>
struct CESetup<>
{
  CESetup(AutoPacket& packet);

  CurrentContextPusher pshr;

  template<int>
  bool Commit(...) { return false; }
};

template<class MemFn, class Index = typename std::make_index_sequence<Decompose<MemFn>::N>>
struct CE;

/// <summary>
/// Specialization for nonmember function calls
/// </summary>
template<class RetType, class... Args, int... N>
struct CE<RetType (*)(Args...), std::index_sequence<N...>>:
  Decompose<RetType(*)(Args...)>
{
  typedef CESetup<Args...> t_ceSetup;
  static const bool deferred = false;

  // This is the true type of the input, it's the fnptr itself, not a function object
  typedef RetType(*t_pfn)(Args...);

  /// <summary>
  /// Binder struct, lets us refer to an instance of Call by type
  /// </summary>
  /// <param name="obj">A shared pointer to the object to be called</param>
  /// <param name="packet">The AutoPacket to be used to satisfy the input arguments for the extractor</param>
  static void Call(const void* pfn, AutoPacket& packet) {
    // Setup, handoff, commit
    t_ceSetup extractor(packet);
    ((t_pfn)pfn)(
      static_cast<typename auto_arg<Args>::arg_type>(std::get<N>(extractor.args))...
    );
    autowiring::noop(extractor.template Commit<N>(false)...);
  }
};

/// <summary>
/// Specialization for member function AutoFilter functions
/// </summary>
template<class T, class... Args, int... N>
struct CE<void (T::*)(Args...), std::index_sequence<N...>> :
  Decompose<void (T::*)(Args...)>
{
  typedef CESetup<Args...> t_ceSetup;
  static const bool deferred = false;

  /// <summary>
  /// Binder struct, lets us refer to an instance of Call by type
  /// </summary>
  template<void(T::*memFn)(Args...)>
  static void Call(const void* pObj, AutoPacket& packet) {
    // Extract, call, commit
    t_ceSetup extractor(packet);
    (((T*) pObj)->*memFn)(
      static_cast<typename auto_arg<Args>::arg_type>(std::get<N>(extractor.args))...
    );
    autowiring::noop(extractor.template Commit<N>(false)...);
  }
};

/// <summary>
/// Specialization for stateless member function AutoFilter routines
/// </summary>
template<class T, class... Args, int... N>
struct CE<void (T::*)(Args...) const, std::index_sequence<N...>> :
  Decompose<void (T::*)(Args...)>
{
  typedef CESetup<Args...> t_ceSetup;
  static const bool deferred = false;

  template<void(T::*memFn)(Args...) const>
  static void Call(const void* pObj, AutoPacket& packet) {
    // Extract, call, commit
    t_ceSetup extractor(packet);
    (((const T*) pObj)->*memFn)(
      static_cast<typename auto_arg<Args>::arg_type>(std::get<N>(extractor.args))...
    );
    autowiring::noop(extractor.template Commit<N>(false)...);
  }
};

/// <summary>
/// Specialization for deferred member function AutoFilter routines
/// </summary>
template<class T, class... Args, int... N>
struct CE<Deferred(T::*)(Args...), std::index_sequence<N...>> :
  Decompose<void (T::*)(Args...)>
{
  typedef CESetup<Args...> t_ceSetup;
  static const bool deferred = true;

  template<Deferred(T::*memFn)(Args...)>
  static void Call(const void* pObj, AutoPacket& autoPacket) {
    // Obtain a shared pointer of the AutoPacket in order to ensure the packet
    // is not destroyed when we pend this lambda to the destination object's
    // dispatch queue.
    auto pAutoPacket = autoPacket.shared_from_this();

    // Pend the call to this object's dispatch queue:
    *(T*) pObj += [pObj, pAutoPacket] {

      // Extract, call, commit
      t_ceSetup extractor(*pAutoPacket);
      (((T*) pObj)->*memFn)(
        static_cast<typename auto_arg<Args>::arg_type>(std::get<N>(extractor.args))...
      );
      autowiring::noop(extractor.template Commit<N>(false)...);
    };
  }
};

}
