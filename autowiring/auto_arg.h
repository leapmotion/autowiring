// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_id.h"
#include "auto_in.h"
#include "auto_out.h"
#include "auto_prev.h"
#include "SharedPointerSlot.h"

class AutoPacket;
class CoreContext;

/*
 The auto_arg<T> classes are used to generate of auto_in and auto_out types
 based on the argument T.
 The core functionality is that all argument types are treated as standard
 types, and can be re-derived from those standard types.
 It is expected that every parent of auto_arg will define:
 - id_type (an unqualified T)
 - base_type (a T qualified by const and/or &)
 - shared_type (the pointer T from which the parent inherits)
 Furthermore, by classifying arguments in terms of orientation and fundamental
 T, auto_arg enables the specification of a principal T to any class
 implementing an AutoFilter method.
 */

/// <summary>
/// Reinterpret copied argument as input
/// </summary>
template<class T>
class auto_arg
{
public:
  typedef const T& type;
  typedef type arg_type;
  typedef auto_id<T> id_type;
  static const bool is_input = true;
  static const bool is_output = false;
  static const bool is_shared = false;
  static const bool is_multi = false;
  static const int tshift = 0;

  template<class C>
  static const T& arg(C& packet) {
    return packet.template Get<T>();
  }
};

/// <summary>
/// Reinterpret copied argument as input
/// </summary>
template<class T>
class auto_arg<const T>:
  public auto_arg<T>
{};

/// <summary>
/// Specialization for "const T&" ~ auto_in<T>
/// </summary>
template<class T>
class auto_arg<const T&> :
  public auto_arg<T>
{};

/// <summary>
/// Specialization for "std::shared_ptr<const T>" ~ auto_in<T>
/// </summary>
template<class T>
class auto_arg<std::shared_ptr<const T>>
{
public:
  typedef const std::shared_ptr<const T>& type;
  typedef type arg_type;
  typedef auto_id<T> id_type;
  static const bool is_input = true;
  static const bool is_output = false;
  static const bool is_shared = true;
  static const bool is_multi = false;
  static const int tshift = 0;

  template<class C>
  static const std::shared_ptr<const T>& arg(C& packet) {
    auto retVal = packet.template GetShared<T>();
    if (!retVal)
      return SharedPointerSlot::null<const T>();
    return *retVal;
  }
};

/// <summary>
/// Specialization for "T*" ~ auto_in<T*>.  T must be const-qualified in order to be an input parameter.
/// </summary>
template<class T>
class auto_arg<T*>
{
public:
  static_assert(std::is_const<T>::value, "Pointer-typed input parameters must point to a const-qualified type (T must be const-qualified)");
  typedef T* type;
  typedef T* arg_type;
  typedef auto_id<T*> id_type;
  static const bool is_input = true;
  static const bool is_output = false;
  static const bool is_shared = false;
  static const bool is_multi = false;
  static const int tshift = 0;

  template<class C>
  static const T* arg(C& packet) {
    return packet.template Get<const T*>();
  }
};

/// <summary>
/// Specialization for equivalent T auto_in<T>
/// </summary>
template<class T>
class auto_arg<auto_in<T>>:
  public auto_arg<T>
{};

namespace Internal {

/// <summary>
/// Construction helper for output-by-reference decoration types
/// </summary>
/// <remarks>
/// If an output decoration type T has a constructor of the form T(AutoPacket&), then this constructor should
/// be invoked preferentially when T is being constructed.
/// </remarks>
template<class T, bool has_default = std::is_constructible<T>::value, bool has_autofilter = std::is_constructible<T, AutoPacket&>::value>
struct auto_arg_ctor_helper;

template<class T, bool has_default>
struct auto_arg_ctor_helper<T, has_default, true> {
  static std::shared_ptr<T> arg(AutoPacket& packet) {
    return std::make_shared<T>(packet);
  }
};

template<class T, bool has_default>
struct auto_arg_ctor_helper<T, has_default, false> {
  static_assert(has_default, "Cannot speculatively construct an output argument of type T, it doesn't have any available constructors");

  template<void* (*)(size_t)>
  struct fn {};

  template<typename U>
  static std::shared_ptr<U> Allocate(fn<&U::operator new>*) {
    return std::shared_ptr<U>(new U);
  }

  template<typename U>
  static std::shared_ptr<U> Allocate(...) {
    return std::make_shared<U>();
  }

  static std::shared_ptr<T> arg(AutoPacket&) {
    // Use make shared, if we can; if static new is present on this type, though, then we have to use
    // the uglier two-part construction syntax
    return Allocate<T>(nullptr);
  }
};

} // end of namespace Internal

/// <summary>
/// Specialization for "T&" ~ auto_out<T>
/// </summary>
template<class T>
class auto_arg<T&>
{
public:
  typedef std::shared_ptr<T> type;

  // Utility type, required to dereference the std::shared_ptr
  struct arg_type {
    arg_type(std::shared_ptr<T>& arg) :
      arg(*arg)
    {}

    T& arg;
    operator T&() const { return arg; }
  };

  typedef auto_id<T> id_type;
  static const bool is_input = false;
  static const bool is_output = true;
  static const bool is_shared = false;
  static const bool is_multi = false;
  static const int tshift = 0;

  static std::shared_ptr<T> arg(AutoPacket& packet) {
    return Internal::auto_arg_ctor_helper<T>::arg(packet);
  }

  template<class C>
  static void Commit (C& packet, type val) {
    packet.template Decorate<T>(val);
  }
};

/// <summary>
/// Specialization for "std::shared_ptr<T>&" ~ auto_in<T>
/// </summary>
template<class T>
class auto_arg<std::shared_ptr<T>&>:
  public auto_arg<T&>
{
public:
  static const bool is_shared = true;

  // Utility type, required to dereference the std::shared_ptr
  struct arg_type {
    arg_type(std::shared_ptr<T>& arg) :
      arg(arg)
    {}

    std::shared_ptr<T>& arg;
    operator std::shared_ptr<T>&() const { return arg; }
  };

  template<class C>
  static std::shared_ptr<T> arg(C&) {
    return std::shared_ptr<T>();
  }
};

/// <summary>
/// Forbidden input T
/// </summary>
template<class T>
class auto_arg<std::shared_ptr<T>> {
  static_assert(
    std::is_same<T, AutoPacket>::value,
    "std::shared_ptr<AutoPacket> is the only T that may be supplied as a non-const shared input"
  );
};

/// <summary>
/// Specialization for equivalent T auto_out<T>
/// </summary>
template<class T>
class auto_arg<auto_out<T>>:
  public auto_arg<T&>
{
public:
  typedef auto_out<T> arg_type;
};

template<class T, int N>
class auto_arg<auto_prev<T, N>>
{
public:
  typedef auto_prev<T, N> type;
  typedef auto_prev<T, N> arg_type;
  typedef auto_id<T> id_type;

  static const bool is_input = true;
  static const bool is_output = false;
  static const bool is_shared = true;
  static const bool is_multi = false;
  static const int tshift = N;

  template<class C>
  static const T* arg(C& packet) {
    const T* retVal;
    packet.template Get<T>(retVal, N);
    return retVal;
  }
};

/// <summary>
/// AutoPacket specialization
/// </summary>
/// <remarks>
/// Because this type is immediately satisfied, it is neither an input nor an output
/// </remarks>
template<>
class auto_arg<AutoPacket&>
{
public:
  typedef AutoPacket& type;
  typedef auto_in<AutoPacket> arg_type;
  typedef AutoPacket id_type;
  static const bool is_input = false;
  static const bool is_output = false;
  static const bool is_shared = false;
  static const bool is_multi = false;
  static const int tshift = 0;

  static AutoPacket& arg(AutoPacket& packet) {
    return packet;
  }
};

/// <summary>
/// CoreContext specialization
/// </summary>
template<>
class auto_arg<CoreContext&>
{
public:
  typedef CoreContext& type;
  typedef CoreContext& arg_type;
  typedef CoreContext id_type;
  static const bool is_input = false;
  static const bool is_output = false;
  static const bool is_shared = false;
  static const bool is_multi = false;
  static const int tshift = 0;

  static CoreContext& arg(AutoPacket&);
};

/// <summary>
/// shared_ptr CoreContext specialization
/// </summary>
template<>
class auto_arg<std::shared_ptr<CoreContext>>
{
public:
  typedef std::shared_ptr<CoreContext> type;
  typedef std::shared_ptr<CoreContext> arg_type;
  typedef CoreContext id_type;
  static const bool is_input = false;
  static const bool is_output = false;
  static const bool is_shared = false;
  static const bool is_multi = false;
  static const int tshift = 0;

  static std::shared_ptr<CoreContext> arg(AutoPacket&);
};

/// <summary>
/// Multi-in specialization
/// </summary>
/// <remarks>
/// This specialization is for gathering multiply decorated types from a packet
/// </remarks>
template<class T>
class auto_arg<T const **>
{
public:
  typedef const T** arg_type;
  struct type {
    type(type&& rhs) :
      ptr(std::move(rhs.ptr))
    {}

    explicit type(std::unique_ptr<const T*[]> ptr) :
      ptr{std::move(ptr)}
    {}

    std::unique_ptr<const T*[]> ptr;

    operator const T**(void) const { return ptr.get(); }
  };

  typedef auto_id<T> id_type;
  static const bool is_input = true;
  static const bool is_output = false;
  static const bool is_shared = false;
  static const bool is_multi = true;
  static const int tshift = 0;

  template<class C>
  static type arg(C& packet) {
    return type{packet.template GetAll<T>()};
  }
};

template<class T>
class auto_arg<T const *const*>:
  public auto_arg<T const**>
{};

/// <summary>
/// Shared pointer multi-in specialization
/// </summary>
template<class T>
class auto_arg<std::shared_ptr<const T>*>
{
public:
  typedef std::shared_ptr<const T>* arg_type;

  struct type {
    type(type&& rhs) :
      ptr(std::move(rhs.ptr))
    {}

    explicit type(std::unique_ptr<std::shared_ptr<const T>[]> ptr) :
      ptr{std::move(ptr)}
    {}

    std::unique_ptr<std::shared_ptr<const T>[]> ptr;

    operator std::shared_ptr<const T>*(void) const { return ptr.get(); }
  };

  typedef auto_id<T> id_type;
  static const bool is_input = true;
  static const bool is_output = false;
  static const bool is_shared = false;
  static const bool is_multi = true;
  static const int tshift = 0;

  template<class C>
  static type arg(C& packet) {
    return type{packet.template GetAllShared<T>()};
  }
};


template<class T>
class auto_arg<const std::shared_ptr<const T>*>:
  public auto_arg<std::shared_ptr<const T>*>
{};

/// <summary>
/// Utility predicate, used to assess whether T is an output argument
/// </summary>
template<class T>
struct arg_is_out {
  static const bool value = auto_arg<T>::is_output;
};
