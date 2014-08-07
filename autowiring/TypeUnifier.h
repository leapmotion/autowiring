// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "has_simple_constructor.h"
#include "Object.h"
#include RVALUE_HEADER

class TypeUnifier: public Object {};

template<class T>
class TypeUnifierComplex:
  public T,
  public TypeUnifier
{
public:
  template<class... Args>
  TypeUnifierComplex(Args&&... args) :
    T(std::forward<Args>(args)...)
  {}
};

/// <summary>
/// Utility class which allows us to either use the pure type T, or a unifier, as appropriate
/// </summary>
template<
  class T,
  bool inheritsObject = std::is_base_of<Object, T>::value
>
struct SelectTypeUnifier;

// Anyone already inheriting Object can just use Object
template<class T>
struct SelectTypeUnifier<T, true> {
  typedef T type;
};

// Otherwise, if there's a complex ctor, we have to use Args
template<class T>
struct SelectTypeUnifier<T, false> {
  typedef TypeUnifierComplex<T> type;
};
