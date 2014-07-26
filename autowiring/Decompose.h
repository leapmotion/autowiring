// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include TYPE_TRAITS_HEADER
#include <typeinfo>

/// <summary>
/// Extended version of is_same
/// </summary>
/// <remarks>
/// Inherits from true_type if T is the same as any of Us...
/// </remarks>
template<typename T, typename... Us>
struct is_any_same;

template<typename T>
struct is_any_same<T> {
  static const bool value = false;
};

template<typename T, typename U, typename... Us>
struct is_any_same<T, U, Us...> {
  static const bool value = std::is_same<T, U>::value || is_any_same<T, Us...>::value;
};

/// <summary>
/// Provides some static reflection support for member function pointers
/// </summary>
template<class MemFn>
struct Decompose;

template<class R, class W, class... Args>
struct Decompose<R(W::*)(Args...)> {
  typedef R (W::*memType)(Args...);
  typedef void fnType(Args...);
  typedef W type;
  typedef R retType;
  static const int N = sizeof...(Args);

  /// <summary>
  /// Returns an array of length N+1 of argument types on the bound type
  /// </summary>
  /// <remarks>
  /// The last entry in this list is guaranteed to be nullptr.  The list's actual size is
  /// one larger than the number of arguments, in order to allow a proper return for a zero-
  /// argument function pointer.
  /// </remarks>
  static const std::type_info* (&Enumerate(void))[N + 1] {
    static const std::type_info* ti[] = {&typeid(Args)..., nullptr};
    return ti;
  }

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
  static const T(&Enumerate(void))[N + 1] {
    static const T rb[] = {typename T::template rebind<Args>()..., T()};
    return rb;
  }
};
