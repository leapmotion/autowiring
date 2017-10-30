// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include "CoreObject.h"
#include RVALUE_HEADER
#include TYPE_TRAITS_HEADER

namespace autowiring {
class TypeUnifier: public CoreObject {};

template<class T>
class TypeUnifierComplex:
  public T,
  public TypeUnifier
{
public:
  TypeUnifierComplex(void) {}

  template<class Arg, class... Args>
  TypeUnifierComplex(Arg&& arg, Args&&... args) :
    T(std::forward<Arg>(arg), std::forward<Args>(args)...)
  {}
};

/// <summary>
/// Utility class which allows us to either use the pure type T, or a unifier, as appropriate
/// </summary>
template<
  class T,
  bool inheritsCoreObject = std::is_base_of<CoreObject, T>::value
>
struct SelectTypeUnifier;

// Anyone already inheriting CoreObject can just use CoreObject
template<class T>
struct SelectTypeUnifier<T, true> {
  typedef T type;
};

// Otherwise, if there's a complex ctor, we have to use Args
template<class T>
struct SelectTypeUnifier<T, false> {
  typedef TypeUnifierComplex<T> type;
};
}
