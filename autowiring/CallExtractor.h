// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_arg.h"
#include "auto_in.h"
#include "auto_tuple.h"
#include "AutoPacket.h"
#include "CurrentContextPusher.h"
#include "Decompose.h"
#include "index_tuple.h"
#include "noop.h"

class Deferred;

namespace autowiring {

// The type of the call centralizer
typedef void(*t_extractedCall)(const void* obj, AutoPacket&);

struct tag_xoutput_t {};
struct tag_xoutput_shared_t {};
struct tag_no_construct_t {};

template<typename T>
struct tag_xoutput_disc {
  typedef tag_xoutput_t tag;
};

template<typename T>
struct tag_xoutput_disc<std::shared_ptr<T>> {
  typedef tag_xoutput_shared_t tag;
};

template<typename T, typename Pack, typename = void>
struct extraction_filter :
  public auto_arg<T>
{
  extraction_filter(Pack&) {}
};

template<typename T, class... Allocated>
struct extraction_filter<
  T,
  autowiring::tuple<Allocated...>,
  typename std::enable_if<!autowiring::find<T, Allocated...>::value>::type
> :
  public auto_arg<T>
{
  extraction_filter(autowiring::tuple<Allocated...>&) {}
};

template<typename T, class... Allocated>
struct extraction_filter<
  T,
  autowiring::tuple<Allocated...>,
  typename std::enable_if<
    autowiring::find<T, Allocated...>::value &&
    auto_arg<T>::is_output
  >::type
> {
  extraction_filter(autowiring::tuple<Allocated...>& allocated) :
    allocated(allocated)
  {}

  autowiring::tuple<Allocated...>& allocated;

  template<typename... Outputs>
  std::shared_ptr<typename std::decay<T>::type> arg(AutoPacket& packet) {
    typedef typename std::decay<T>::type TActual;
    auto& value = autowiring::get<T>(allocated);

    // Do-nothing shared pointer for this value, because it's externally allocated
    return std::shared_ptr<TActual>{
      &value,
      [] (TActual*) {}
    };
  }
};

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

  template<typename... Allocated>
  CESetup(AutoPacket& packet, autowiring::tuple<Allocated...>& allocated) :
    packet(packet),
    pshr(packet.GetContext()),
    args(extraction_filter<Args, autowiring::tuple<Allocated...>>(allocated).arg(packet)...)
  {}

  AutoPacket& packet;
  CurrentContextPusher pshr;
  autowiring::tuple<typename auto_arg<Args>::type...> args;

  template<int N>
  typename std::enable_if<
    auto_arg<typename autowiring::nth_type<N, Args...>::type>::is_output,
    bool
  >::type Commit(bool) {
    auto_arg<typename autowiring::nth_type<N, Args...>::type>::Commit(packet, autowiring::get<N>(args));
    return true;
  }

  template<int N>
  bool Commit(...) { return false; }

  template<typename Arg>
  Arg& ExtractInternal(const tag_xoutput_t&) { return *autowiring::get<std::shared_ptr<Arg>>(args); }

  template<typename Arg>
  Arg& ExtractInternal(const tag_xoutput_shared_t&) { return autowiring::get<Arg>(args); }

  template<typename Arg>
  void Extract(Arg& arg) {
    arg = ExtractInternal<Arg>(typename tag_xoutput_disc<Arg>::tag{});
  }
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

template<class MemFn, class Index = typename make_index_tuple<Decompose<MemFn>::N>::type>
struct CE;

/// <summary>
/// Specialization for nonmember function calls
/// </summary>
template<class RetType, class... Args, int... N>
struct CE<RetType (*)(Args...), index_tuple<N...>>:
  Decompose<RetType(*)(Args...)>
{
  typedef CESetup<Args...> t_ceSetup;
  static const bool has_outputs = is_any<auto_arg<Args>::is_output...>::value;
  static const bool deferred = false;
  static const bool stateless = true;

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
      static_cast<typename auto_arg<Args>::arg_type>(autowiring::get<N>(extractor.args))...
    );
    autowiring::noop(extractor.template Commit<N>(false)...);
  }
  
  static void CallWithArgs(void* pfn, t_ceSetup& pack) {
    // Extract, call, commit
    ((t_pfn)pfn)(
      static_cast<typename auto_arg<Args>::arg_type>(autowiring::get<N>(pack.args))...
    );
  }
};

/// <summary>
/// Specialization for member function AutoFilter functions
/// </summary>
template<class T, class... Args, int... N>
struct CE<void (T::*)(Args...), index_tuple<N...>> :
  Decompose<void (T::*)(Args...)>
{
  typedef CESetup<Args...> t_ceSetup;
  static const bool has_outputs = is_any<auto_arg<Args>::is_output...>::value;
  static const bool stateless = false;
  static const bool deferred = false;

  /// <summary>
  /// Binder struct, lets us refer to an instance of Call by type
  /// </summary>
  template<void(T::*memFn)(Args...)>
  static void Call(const void* pObj, AutoPacket& packet) {
    // Extract, call, commit
    t_ceSetup extractor(packet);
    (((T*) pObj)->*memFn)(
      static_cast<typename auto_arg<Args>::arg_type>(autowiring::get<N>(extractor.args))...
    );
    autowiring::noop(extractor.template Commit<N>(false)...);
  }

  template<void(T::*memFn)(Args...)>
  static void CallWithArgs(const void* pObj, t_ceSetup& pack) {
    // Extract, call, commit
    (((T*)pObj)->*memFn)(
      static_cast<typename auto_arg<Args>::arg_type>(autowiring::get<N>(pack.args))...
    );
  }
};

/// <summary>
/// Specialization for stateless member function AutoFilter routines
/// </summary>
template<class T, class... Args, int... N>
struct CE<void (T::*)(Args...) const, index_tuple<N...>> :
  Decompose<void (T::*)(Args...)>
{
  typedef CESetup<Args...> t_ceSetup;
  static const bool has_outputs = is_any<auto_arg<Args>::is_output...>::value;
  static const bool stateless = true;
  static const bool deferred = false;
  
  template<void(T::*memFn)(Args...) const>
  static void Call(const void* pObj, AutoPacket& packet) {
    // Extract, call, commit
    t_ceSetup extractor(packet);
    (((const T*) pObj)->*memFn)(
      static_cast<typename auto_arg<Args>::arg_type>(autowiring::get<N>(extractor.args))...
    );
    autowiring::noop(extractor.template Commit<N>(false)...);
  }

  template<void(T::*memFn)(Args...) const>
  static void CallWithArgs(const void* pObj, t_ceSetup& pack) {
    // Extract, call, commit
    (((const T*)pObj)->*memFn)(
      static_cast<typename auto_arg<Args>::arg_type>(autowiring::get<N>(pack.args))...
    );
  }
};

/// <summary>
/// Specialization for deferred member function AutoFilter routines
/// </summary>
template<class T, class... Args, int... N>
struct CE<Deferred(T::*)(Args...), index_tuple<N...>> :
  Decompose<void (T::*)(Args...)>
{
  typedef CESetup<Args...> t_ceSetup;
  static const bool has_outputs = is_any<auto_arg<Args>::is_output...>::value;
  static const bool stateless = false;
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
        static_cast<typename auto_arg<Args>::arg_type>(autowiring::get<N>(extractor.args))...
      );
      autowiring::noop(extractor.template Commit<N>(false)...);
    };
  }
};

}