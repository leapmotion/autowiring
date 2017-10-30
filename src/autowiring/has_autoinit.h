// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include "Decompose.h"

namespace autowiring {

template<class T>
struct has_valid_autoinit {
  template<class Fn, Fn>
  struct unnamed_constant;

  template<class U>
  static std::false_type select(...);

  template<class U>
  static std::true_type select(unnamed_constant<decltype(&U::AutoInit), &U::AutoInit>*);

  // Conveninece typedef used externally:
  typedef decltype(select<T>(nullptr)) has_valid;

  // Evaluates to true only if T includes a unique AutoFilter method with at least one argument.
  static const bool value = has_valid::value;
};

/// <summary>
/// Detects whether the specified type T has a method with the name AutoInit
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
template<class T>
struct has_autoinit : has_valid_autoinit<T>::has_valid {};

template<class T>
static void CallAutoInit(T& obj, std::true_type) {
  obj.AutoInit();
}

template<class T>
static void CallAutoInit(T&, std::false_type) {}

}
