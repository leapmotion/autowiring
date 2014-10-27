// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include TYPE_TRAITS_HEADER
#include <typeinfo>

template<class... Ts>
struct TemplatePack {
  static const int N = sizeof...(Ts);

  /// <returns>
  /// An array of type T, parameterized by the bound function's arguments
  /// </returns>
  /// <remarks>
  /// Type T must be move-constructable.
  ///
  /// The returned array contains one more element than the arity of the decomposed member function
  /// type.  Each element in the array is initialized based on the type of the corresponding argument
  /// in the decomposed function.  Elements in the array are constructed using the "rebind" structure
  /// which must be an interior type to type T.  An instance of type rebind should be castable to the
  /// base type T, or it must be a function returning a value of type T.
  /// </remarks>
  template<class T>
  struct Enumerate {
    static const T types[N + 1];
  };
};

template<class... Ts>
template<class T>
const T TemplatePack<Ts...>::Enumerate<T>::types[] = {typename T::template rebind<Ts>()..., T()};

/// <summary>
/// Provides some static reflection support for member function pointers
/// </summary>
template<class MemFn>
struct Decompose;

template<class R, class W, class... Args>
struct Decompose<R(W::*)(Args...)> :
  TemplatePack<Args...>
{
  typedef R(W::*memType)(Args...);
  typedef void fnType(Args...);
  typedef W type;
  typedef R retType;
};

template<class R, class... Args>
struct Decompose<R(*)(Args...)> :
  TemplatePack<Args...>
{
  typedef void fnType(Args...);
  typedef void type;
  typedef R retType;
};
