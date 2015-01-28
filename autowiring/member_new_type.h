// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "Decompose.h"
#include STL_TUPLE_HEADER

namespace autowiring {

enum class factorytype {
  // No factory method in this class
  none,

  // IType* New(void);
  // std::tuple<IType1, IType2> New(void);
  ret_val,

  // void New(IType1*&, IType2*&);
  multi_byref
};

/// <summary>
/// Decomposes the member function type into a specific supported return type
/// </summary>
template<class MemFn>
struct sel_valuetype {
  static const factorytype value = factorytype::none;
};

template<class T, class U>
struct sel_valuetype<T* (U::*)()> {
  static const factorytype value = factorytype::ret_val;
};

template<class T, class U>
struct sel_valuetype<T* (U::*)() const> {
  static const factorytype value = factorytype::ret_val;
};

template<class... Ts, class U>
struct sel_valuetype<std::tuple<Ts*...>(U::*)()> {
  static const factorytype value = factorytype::ret_val;
};

template<class... Ts, class U>
struct sel_valuetype<std::tuple<Ts*...>(U::*)() const> {
  static const factorytype value = factorytype::ret_val;
};

template<class... Ts, class U>
struct sel_valuetype<void (U::*)(Ts*&...)> {
  static const factorytype value = factorytype::multi_byref;
};

template<class... Ts, class U>
struct sel_valuetype<void (U::*)(Ts*&...) const> {
  static const factorytype value = factorytype::multi_byref;
};

template<factorytype ft>
struct factorytype_const {
  static const factorytype value = ft;
};
  
template<class T>
struct member_new_type_helper {
  template<class U>
  static factorytype_const<factorytype::none> select(...);

  template<class U>
  static factorytype_const<sel_valuetype<decltype(&U::New)>::value> select(void*);

  // Conveninece typedef used externally:
  typedef decltype(select<T>(nullptr)) factory_value_type;

  // Evaluates to true only if T includes a unique AutoFilter method with at least one argument.
  static const factorytype value = factory_value_type::value;
};

/// <summary>
/// Detects whether the specified type T has a nonstatic factory New method
/// </summary>
/// <remarks>
/// An auto-initializer routine is called after a shared pointer assignment has taken place, but before
/// an entity is injected into the context.  This affords a context member an opportunity to obtain and,
/// optionally , distribute its own shared pointer before that entity becomes discoverable in that
/// context.
///
/// Note that this call is actually made before the type is actually injected in the context, so an
/// attempt to perform Autowired<T> from AutoInit will necessarily fail.
/// </remarks>
template<class T, factorytype ft = member_new_type_helper<T>::value>
struct member_new_type
{
  static const factorytype factory = ft;
};

template<class T>
struct member_new_type<T, factorytype::ret_val> {
  static const factorytype factory = factorytype::ret_val;
};

}
