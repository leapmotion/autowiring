// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "Decompose.h"
#include "Deferred.h"
#include "is_any.h"

//=======================================
// Test whether return type is meaningful
//=======================================

/// <summary>
/// Determines whether the return value of a function is allowed for an AutoFilter:
/// - void
/// - Deferred
/// </summary>
template<class W, bool Selector = true>
struct has_autofilter_return :
  std::integral_constant<bool,
    std::is_same<void, typename Decompose<decltype(&W::AutoFilter)>::retType>::value ||
    std::is_same<Deferred, typename Decompose<decltype(&W::AutoFilter)>::retType>::value
  >
{};

// Specialization pertains only when W has no AutoFilter method.
template<class W>
struct has_autofilter_return<W, false>:
  std::false_type
{};

//==========================================
// Test whether AutoFilter has > 0 arguments
//==========================================

// IMPORTANT: This evaluation must occur only when it has been
// determined that the class has a unique AutoFilter method.
template<class W, bool Selector>
struct has_autofilter_arity:
  std::integral_constant<bool,
    Decompose<decltype(&W::AutoFilter)>::N != 0
  >
{
  static const int N = Decompose<decltype(&W::AutoFilter)>::N;
};

// Specialization pertains only when W has no AutoFilter method.
template<class W>
struct has_autofilter_arity<W, false>:
  std::false_type
{
  static const int N = -1;
};

//===========================================================
// Test for existence of unique AutoFilter with > 0 arguments
//===========================================================

// Evaluates with value == true when there is a unique AutoFilter method taking at least one argument.
// Evaluates with value == false when:
// - There is no AutoFilter method
// - There is an AutoFilter(void) method
// - The AutoFilter method repeats arguments
// - There is more than one AutoFilter method
template<typename T>
struct has_unambiguous_autofilter
{
  template<class Fn, Fn>
  struct unnamed_constant;

  // Resolves only when class U has no unqiue method named AutoFilter.
  template<class U>
  static std::false_type select(...);

  // Specialization when class U has a unique method named AutoFilter.
  // IMPORTANT: When more than one AutoFilter method exists, this specialization will fail.
  template<class U>
  static std::true_type select(unnamed_constant<decltype(&U::AutoFilter), &U::AutoFilter>*);

  // Evaluates to true only if T includes a unique AutoFilter method,
  // with a meaningful return type,
  // with at least one argument
  static const bool value =
      has_autofilter_arity<
        T,
        has_autofilter_return<
          T,
          decltype(select<T>(nullptr))::value
        >::value
      >::value;
};

class AutoPacket;

// Inheriting from this class ensures the existence of a valid AutoFilter method.
// This is used to distinguish between a no AutoFilter methods
// and multiple AutoFilter definitions.
struct test_valid_autofilter {
  void AutoFilter(AutoPacket&);
};

/// <summary>
/// Reflection primitive which can be used to detect a unique "AutoFilter" method present on type T.
/// </summary>
template<typename T>
struct has_autofilter {

  // Evaluates to false when T does not include an AutoFilter method with at least one argument.
  static const bool value = has_unambiguous_autofilter<T>::value;

  // This class has at least one AutoFilter method
  struct detect_ambiguous_autofilter: T, test_valid_autofilter {};

  // Ensures a compiler error when the identification of T::AutoFilter is ambiguous.
  // This cannot be in has_unambiguous_autofilter, since that would be recursive.
  static_assert(
    value ||
    has_unambiguous_autofilter<detect_ambiguous_autofilter>::value,
    "Cannot define more than one AutoFilter method and all AutoFilter methods must be public and all AutoFilter reference-output arguments must be completely defined"
  );
};
