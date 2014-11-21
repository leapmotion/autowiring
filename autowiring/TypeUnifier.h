// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "has_autofilter.h"
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
  TypeUnifierComplex(void) {}

  template<class Arg, class... Args>
  TypeUnifierComplex(Arg&& arg, Args&&... args) :
    T(std::forward<Arg>(arg), std::forward<Args>(args)...)
  {}
};

template<class TrueType, class MemFn>
struct TypeUnifierComplexAutoFilter;

template<class TrueType, class T, class RetType, class... Args>
class TypeUnifierComplexAutoFilter<TrueType, RetType(T::*)(Args...)>:
  public TrueType,
  public TypeUnifier
{
public:
  TypeUnifierComplexAutoFilter(void) {}

  template<class ArgsHead, class... ArgsTail>
  TypeUnifierComplexAutoFilter(ArgsHead&& argsh, ArgsTail&&... argst) :
    T(std::forward<ArgsHead>(argsh), std::forward<ArgsTail>(argst)...)
  {}

  /// <summary>
  /// Fixup AutoFilter call
  /// </summary>
  /// <remarks>
  /// This is necessary because nonpolymorphic types cannot be held by an AnySharedPointer type,
  /// and so we must instead hold a pointer to the type unifier instead of to the proper type.
  /// Thus, in order to ensure that the nonpolymorphic instance is called with a correct value
  /// of "this", this trivial forwarding routine is used to perform the fixup for us.
  /// </remarks>
  RetType AutoFilter(Args... args) {
    return T::AutoFilter(std::forward<Args>(args)...);
  }
};

/// <summary>
/// Utility class which allows us to either use the pure type T, or a unifier, as appropriate
/// </summary>
template<
  class T,
  bool inheritsObject = std::is_base_of<Object, T>::value,
  bool has_autofilter = has_autofilter<T>::value
>
struct SelectTypeUnifier;

// Anyone already inheriting Object can just use Object
template<class T, bool has_autofilter>
struct SelectTypeUnifier<T, true, has_autofilter> {
  typedef T type;
};

// Otherwise, if there's a complex ctor, we have to use Args
template<class T>
struct SelectTypeUnifier<T, false, false> {
  typedef TypeUnifierComplex<T> type;
};


// Otherwise, if there's a complex ctor, we have to use Args
template<class T>
struct SelectTypeUnifier<T, false, true>
{
  typedef TypeUnifierComplexAutoFilter<T, decltype(&T::AutoFilter)> type;
};
