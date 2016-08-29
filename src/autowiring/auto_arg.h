// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_id.h"

class AutoPacket;
class CoreContext;

namespace autowiring {

template <class T> class auto_in;

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
  typedef auto_id_t<T> id_type;
  static const bool is_input = true;
  static const bool is_output = false;
  static const bool is_rvalue = false;
  static const bool is_shared = false;
  static const bool is_multi = false;
  static const int tshift = 0;

  template<class C>
  static const T& arg(C& packet) {
    (void) auto_id_t_init<T, true>::init;
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
class auto_arg<const T&>
{
public:
  typedef const T& type;
  typedef type arg_type;
  typedef auto_id_t<T> id_type;
  static const bool is_input = true;
  static const bool is_output = false;
  static const bool is_rvalue = false;
  static const bool is_shared = false;
  static const bool is_multi = false;
  static const int tshift = 0;

  template<class C>
  static const T& arg(C& packet) {
    (void)auto_id_t_init<T, false>::init;
    return packet.template Get<T>();
  }
};

/// <summary>
/// Specialization for "T&&" ~ auto_in<T&&> and auto_out<T&&>.
/// </summary>
template<class T>
class auto_arg<T&&>
{
public:
  typedef T&& type;
  typedef type arg_type;
  typedef auto_id_t<T> id_type;
  static const bool is_input = true;
  static const bool is_output = true;
  static const bool is_rvalue = true;
  static const bool is_shared = false;
  static const bool is_multi = false;
  static const int tshift = 0;

  template<class C>
  static T&& arg(C& packet) {
    (void) auto_id_t_init<T, false>::init;
    return packet.template GetRvalue<T>();
  }

  template<class C>
  static void Commit(C& packet, T& val) {
    // Do nothing. Modify val in place, no need to commit
  }
};

/// <summary>
/// Specialization for "std::shared_ptr<const T>" ~ auto_in<T>
/// </summary>
template<class T>
class auto_arg<std::shared_ptr<const T>>
{
public:
  typedef const std::shared_ptr<const T>& type;
  typedef type arg_type;
  typedef auto_id_t<T> id_type;
  static const bool is_input = true;
  static const bool is_output = false;
  static const bool is_rvalue = false;
  static const bool is_shared = true;
  static const bool is_multi = false;
  static const int tshift = 0;

  template<class C>
  static const std::shared_ptr<const T>& arg(C& packet) {
    (void) auto_id_t_init<T, false>::init;

    static const std::shared_ptr<const T> null;
    auto retVal = packet.template GetShared<T>();
    if (!retVal)
      return null;
    return *retVal;
  }
};

/// <summary>
/// Specialization for "const std::shared_ptr<const T>&" ~ auto_in<T>
/// </summary>
template<class T>
class auto_arg<const std::shared_ptr<const T>&>:
  public auto_arg<std::shared_ptr<const T>>
{};

/// <summary>
/// Specialization for "std::shared_ptr<T>&&" ~ auto_in<T&&> and auto_out<T&&>.
/// </summary>
template<class T>
class auto_arg<std::shared_ptr<T>&&>
{
public:
  typedef std::shared_ptr<T>&& type;
  typedef type arg_type;
  typedef auto_id_t<T> id_type;
  static const bool is_input = true;
  static const bool is_output = true;
  static const bool is_rvalue = true;
  static const bool is_shared = true;
  static const bool is_multi = false;
  static const int tshift = 0;

  template<class C>
  static std::shared_ptr<T>&& arg(C& packet) {
    (void) auto_id_t_init<T, false>::init;
    return packet.template GetRvalueShared<T>();
  }

  template<class C>
  static void Commit(C& packet, std::shared_ptr<T> val) {
    if (!val)
      packet.template RemoveDecoration<T>();
  }
};

/// <summary>
/// Specialization for "T*" ~ auto_in<T*>.  T must be const-qualified in order to be an input parameter.
/// </summary>
template<class T>
class auto_arg<T*>
{
public:
  typedef std::shared_ptr<T> type;

  // Utility type, required to dereference the std::shared_ptr
  struct arg_type {
    arg_type(const std::shared_ptr<T>& arg) :
      arg(arg.get())
    {}

    T* arg;
    operator T*() const { return arg; }
  };

  typedef auto_id_t<T> id_type;
  static const bool is_input = false;
  static const bool is_output = true;
  static const bool is_rvalue = false;
  static const bool is_shared = false;
  static const bool is_multi = false;
  static const int tshift = 0;

  static std::shared_ptr<T> arg(AutoPacket& packet) {
    (void)auto_id_t_init<T>::init;

    if (!packet.template HasSubscribers<T>())
      return nullptr;
    return detail::auto_arg_ctor_helper<T>::arg(packet);
  }

  template<class C>
  static void Commit(C& packet, type val) {
    packet.template Decorate<T>(val);
  }
};

/// <summary>
/// Specialization for "const T*", which is functionally identical to a reference input
/// </summary>
template<class T>
class auto_arg<const T*>
{
public:
  typedef const T* type;
  typedef type arg_type;
  typedef auto_id_t<T> id_type;
  static const bool is_input = true;
  static const bool is_output = false;
  static const bool is_rvalue = false;
  static const bool is_shared = false;
  static const bool is_multi = false;
  static const int tshift = 0;

  template<class C>
  static const T* arg(C& packet) {
    (void)auto_id_t_init<T, false>::init;
    return &packet.template Get<T>();
  }
};

namespace detail {

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

} // end of namespace detail

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

  typedef auto_id_t<T> id_type;
  static const bool is_input = false;
  static const bool is_output = true;
  static const bool is_rvalue = false;
  static const bool is_shared = false;
  static const bool is_multi = false;
  static const int tshift = 0;

  static std::shared_ptr<T> arg(AutoPacket& packet) {
    // Need to ensure the identifier is initialized properly, we can do a full
    // initialization because this is a byref output type
    (void) auto_id_t_init<T>::init;
    return detail::auto_arg_ctor_helper<T>::arg(packet);
  }

  template<class C>
  static void Commit(C& packet, type val) {
    packet.template Decorate<T>(val);
  }
};

/// <summary>
/// Specialization for "std::shared_ptr<T>&" ~ auto_out<T>
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
    (void) auto_id_t_init<T, false>::init;
    return std::shared_ptr<T>();
  }
};

/// <summary>
/// Specialization for "std::shared_ptr<T>*" ~ auto_out<T>
/// </summary>
template<class T>
class auto_arg<std::shared_ptr<T>*>
{
public:
  struct downstream_status {
    downstream_status(bool has_downstream) :
      has_downstream(has_downstream)
    {}

    bool has_downstream;
    std::shared_ptr<T> arg;

    operator std::shared_ptr<T>*() {
      return has_downstream ? &arg : nullptr;
    }
  };

  typedef std::shared_ptr<T>* arg_type;
  typedef downstream_status type;
  typedef auto_id_t<T> id_type;
  static const bool is_input = false;
  static const bool is_output = true;
  static const bool is_rvalue = false;
  static const bool is_shared = true;
  static const bool is_multi = false;
  static const int tshift = 0;

  template<class C>
  static std::shared_ptr<T> arg(C&) {
    (void)auto_id_t_init<T, false>::init;
    if (!packet.template HasSubscribers<T>())
      return nullptr;
    return std::shared_ptr<T>();
  }

  static downstream_status arg(AutoPacket& packet) {
    (void)auto_id_t_init<T>::init;
    return{ packet.template HasSubscribers<T>() };
  }

  template<class C>
  static void Commit(C& packet, downstream_status val) {
    if(val.has_downstream)
      packet.template Decorate<T>(val.arg);
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
/// CoreContext specialization
/// </summary>
template<>
class auto_arg<CoreContext&>
{
public:
  typedef CoreContext& type;
  typedef CoreContext& arg_type;
  typedef auto_id_t<CoreContext> id_type;
  static const bool is_input = false;
  static const bool is_output = false;
  static const bool is_rvalue = false;
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
  typedef auto_id_t<CoreContext> id_type;
  static const bool is_input = false;
  static const bool is_output = false;
  static const bool is_rvalue = false;
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

  typedef auto_id_t<T> id_type;
  static const bool is_input = true;
  static const bool is_output = false;
  static const bool is_rvalue = false;
  static const bool is_shared = false;
  static const bool is_multi = true;
  static const int tshift = 0;

  template<class C>
  static type arg(C& packet) {
    (void) auto_id_t_init<T, false>::init;
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

  typedef auto_id_t<T> id_type;
  static const bool is_input = true;
  static const bool is_output = false;
  static const bool is_rvalue = false;
  static const bool is_shared = false;
  static const bool is_multi = true;
  static const int tshift = 0;

  template<class C>
  static type arg(C& packet) {
    (void) auto_id_t_init<T, false>::init;
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

}
