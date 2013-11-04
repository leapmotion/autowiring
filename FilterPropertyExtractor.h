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

/// <summary>
/// Utility class which allows discovery of the named 
/// </summary>
template<class T, bool hasFilter = has_autofilter<T>::value>
struct RecipientPropertyExtractor {
  /// <summary>
  /// Passes detected filter inputs to the specified functional
  /// </summary>
  /// <param name="fx">A function of the form fx(const std::type_info&)</param>
  /// <remarks>
  /// This method can be called on any type, and will pass the arguments of the
  /// single detected "Filter" method, one at a time, to the passed functional.
  /// </remarks>
  template<class _Fx>
  static void Enumerate(_Fx&& fx) {
    for(auto ppCur = Decompose<decltype(&T::AutoFilter)>::Enumerate(); *ppCur; ppCur++)
      fx(**ppCur);
  }

  /// <summary>
  /// Utility method which returns argument types in a vector instead of a functional
  /// </summary>
  static std::vector<const std::type_info*> Enumerate(void) {
    auto& c = Decompose<decltype(&T::AutoFilter)>::Enumerate();
    return
      std::vector<const std::type_info*>(
        c,
        c + ARRAYCOUNT(c) - 1
      );
  }
};

template<class T>
struct RecipientPropertyExtractor<T, false> {
  template<class _Fx>
  static void Enumerate(_Fx&&) {}

  static std::vector<const std::type_info*> Enumerate(void) {
    return std::vector<const std::type_info*>();
  }
};