#pragma once
#include "Decompose.h"

//==========================================
// Test whether AutoFilter has > 0 arguments
//==========================================

// Resolves only when Selector is not of type std::false_type
template<class W, class Selector>
struct has_valid_autofilter {
  // Evaluates to false for W::AutoFilter(void), else to true.
  static const bool value = Decompose<decltype(&W::AutoFilter)>::N != 0;
};

// Specialization pertains only when W has no AutoFilter method.
template<class W>
struct has_valid_autofilter<W, std::false_type> {
  static const bool value = false;
};

//====================================================
// Test for existence of AutoFilter with > 0 arguments
//====================================================

template<typename T>
struct has_unambiguous_autofilter
{
  template<class Fn, Fn>
  struct unnamed_constant;

  // Resolves only when class U has no method named AutoFilter
  template<class U>
  static std::false_type select(...);

  // Specialization when class U has a unique method named AutoFilter
  // When more than one AutoFilter method exists, this specialization will fail.
  template<class U>
  static std::true_type select(unnamed_constant<decltype(&U::AutoFilter), &U::AutoFilter>*);

  // Evaluates to true only if T includes a unique AutoFilter method with at least one argument.
  static const bool value = has_valid_autofilter<T, decltype(select<T>(nullptr))>::value;
};

//====================================================
// Test for existence of more than 1 AutoFilter method
//====================================================

class AutoPacket;

// Inheriting from this method ensures the existence of a valid AutoFilter method
struct test_valid_autofilter {
  void AutoFilter(AutoPacket&);
};

/// <summary>
/// Reflection primitive which can be used to detect a unique "AutoFilter" method present on type T
/// </summary>
template<typename T>
struct has_autofilter {

  // Evaluates to false when T does not include an AutoFilter method with at least one argument.
  static const bool value = has_unambiguous_autofilter<T>::value;

  // This class has at least one AutoFilter method
  struct detect_ambiguous_autofilter : T, test_valid_autofilter {};

  // Ensures a compiler error when the identification of T::AutoFilter is ambiguous
  static_assert(value || has_unambiguous_autofilter<detect_ambiguous_autofilter>::value, "Cannot define more than 1 AutoFilter method");
};
