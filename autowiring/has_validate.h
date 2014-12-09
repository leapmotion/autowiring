// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "Decompose.h"
#include <iostream>

template<class T>
struct has_validate_function {
  template<class Fn, Fn>
  struct unnamed_constant;

  template<class U>
  static std::false_type select(...);

  template<class U>
  static std::true_type select(unnamed_constant<decltype(&U::Validate), &U::Validate>*);

  // Conveninece typedef used externally:
  typedef decltype(select<T>(nullptr)) has_valid;

  // Evaluates to true only if T includes a unique AutoFilter method with at least one argument.
  static const bool value = has_valid::value;
};

/// <summary>
/// Detects whether the specified type T has a static method with the name Validate
/// </summary>
template<class T>
struct has_validate : has_validate_function<T>::has_valid {};

template<class Object, class Validator>
static bool CallValidate(const Object& obj, std::true_type) {
  return Validator::Validate(obj);
}

template<class Object, class Validator>
static bool CallValidate(const Object& obj, std::false_type) {
  return true;
}

