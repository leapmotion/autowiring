#pragma once
#include "Decompose.h"

/// <summary>
/// Reflection primitive which can be used to detect a "Filter" method present on type T
/// </summary>
template<typename T>
struct has_autofilter
{
  template<class Fn, Fn>
  struct unnamed_constant;

  template<class U>
  static int select(unnamed_constant<decltype(&U::AutoFilter), &U::AutoFilter>*);

  template<class U>
  static char select(...);

  static const bool value = sizeof(select<T>(nullptr)) == sizeof(int);
};
