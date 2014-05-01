#pragma once
#include "CreationRules.h"
#include "Object.h"

class TypeUnifier: public Object {};

template<class T>
class TypeUnifierComplex:
  public T,
  public TypeUnifier
{
public:
  template<class Arg0, class... Args>
  TypeUnifierComplex(Arg0&& arg0, Args&&... args) :
    T(std::forward<Arg0>(arg0), std::forward<Args>(args)...)
  {}
};

template<class T>
class TypeUnifierSimple:
  public T,
  public TypeUnifier
{
};

/// <summary>
/// Utility class which allows us to either use the pure type T, or a unifier, as appropriate
/// </summary>
template<
  class T,
  bool inheritsObject = std::is_base_of<Object, T>::value,
  bool hasSimpleCtor = has_simple_constructor<T>::value
>
struct SelectTypeUnifier;

// Anyone already inheriting Object can just use Object
template<class T, bool hasSimpleCtor>
struct SelectTypeUnifier<T, true, hasSimpleCtor> {
  typedef T type;
};

// If T has a simple ctor, we don't want to confuse the compiler with the complex TypeUnifier
template<class T>
struct SelectTypeUnifier<T, false, true> {
  typedef TypeUnifierSimple<T> type;
};

// Otherwise, if there's a complex ctor, we have to use Args
template<class T>
struct SelectTypeUnifier<T, false, false> {
  typedef TypeUnifierComplex<T> type;
};
