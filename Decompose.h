#pragma once
#include <typeinfo>

/// <summary>
/// Provides some static reflection support for member function pointers
/// </summary>
template<class MemFn>
struct Decompose;

template<class R, class W>
struct Decompose<R(W::*)()> {
  typedef R (W::*memType)();
  typedef void fnType();
  typedef W type;
  typedef R retType;
  static const int N = 0;

  /// <summary>
  /// Returns an array of length N+1 of argument types on the bound type
  /// </summary>
  /// <remarks>
  /// The last entry in this list is guaranteed to be nullptr.  The list's actual size is
  /// one larger than the number of arguments, in order to allow a proper return for a zero-
  /// argument function pointer.
  /// </remarks>
  static const std::type_info* (&Enumerate(void))[N + 1] {
    static const std::type_info* ti[] = {nullptr};
    return ti;
  }
};

template<class T, class MemFn, MemFn memfn, int arity = Decompose<MemFn>::N>
struct BoundCall;

template<class R, class W, class Arg1>
struct Decompose<R(W::*)(Arg1)> {
  typedef Arg1 t_arg1;
  typedef R(W::*memType)(Arg1);
  typedef void fnType(Arg1);
  typedef W type;
  typedef R retType;
  static const int N = 1;

  static const std::type_info* (&Enumerate(void))[N + 1] {
    static const std::type_info* ti[] = {&typeid(Arg1), nullptr};
    return ti;
  }
};

template<class R, class W, class Arg1, class Arg2>
struct Decompose<R(W::*)(Arg1, Arg2)> {
  typedef Arg1 t_arg1;
  typedef Arg2 t_arg2;
  typedef R(W::*memType)(Arg1, Arg2);
  typedef void fnType(Arg1, Arg2);
  typedef W type;
  typedef R retType;
  static const int N = 2;

  static const std::type_info* (&Enumerate(void))[N + 1] {
    static const std::type_info* ti[] = {&typeid(Arg1), &typeid(Arg2), nullptr};
    return ti;
  }
};

template<class R, class W, class Arg1, class Arg2, class Arg3>
struct Decompose<R(W::*)(Arg1, Arg2, Arg3)> {
  typedef Arg1 t_arg1;
  typedef Arg2 t_arg2;
  typedef Arg3 t_arg3;
  typedef R(W::*memType)(Arg1, Arg2, Arg3);
  typedef void fnType(Arg1, Arg2, Arg3);
  typedef W type;
  typedef R retType;
  static const int N = 3;

  static const std::type_info* (&Enumerate(void))[N + 1] {
    static const std::type_info* ti[] = {&typeid(Arg1), &typeid(Arg2), &typeid(Arg3), nullptr};
    return ti;
  }
};

template<class R, class W, class Arg1, class Arg2, class Arg3, class Arg4>
struct Decompose<R(W::*)(Arg1, Arg2, Arg3, Arg4)> {
  typedef Arg1 t_arg1;
  typedef Arg2 t_arg2;
  typedef Arg3 t_arg3;
  typedef Arg4 t_arg4;
  typedef R(W::*memType)(Arg1, Arg2, Arg3, Arg4);
  typedef void fnType(Arg1, Arg2, Arg3, Arg4);
  typedef W type;
  typedef R retType;
  static const int N = 4;

  static const std::type_info* (&Enumerate(void))[N + 1] {
    static const std::type_info* ti[] = {&typeid(Arg1), &typeid(Arg2), &typeid(Arg3), &typeid(Arg4), nullptr};
    return ti;
  }
};

template<class T, class MemFn, MemFn memfn>
struct BoundCall<T, MemFn, memfn, 0>
{
  typedef Decompose<MemFn> t_decompose;

  /// <summary>
  /// Performs a call on the specified object with the specified member function
  /// <summary>
  /// <remarks>
  /// The passed type object, the last parameter, must be a type repository.  It must support
  /// a method of the following form:
  ///
  /// template<class T>
  /// const T& Get();
  ///
  /// The method must return the object of the desired type, or else it must throw an exception.
  /// It is the caller's responsibility to ensure that all of the necessary types are available
  /// before this call is made, in order to prevent such an exception from being thrown.
  /// </remarks>
  static void Call(typename t_decompose::type* pObj, const T&) {
    (pObj->*memfn)();
  }
};

template<class T, class MemFn, MemFn memfn>
struct BoundCall<T, MemFn, memfn, 1>:
  Decompose<MemFn>
{
  typedef Decompose<MemFn> t_decompose;

  static void Call(typename t_decompose::type* pObj, const T& repo) {
    const typename t_decompose::t_arg1* arg1;
    repo.Get(arg1);
    (pObj->*memfn)(*arg1);
  }
};

template<class T, class MemFn, MemFn memfn>
struct BoundCall<T, MemFn, memfn, 2>:
  Decompose<MemFn>
{
  typedef Decompose<MemFn> t_decompose;

  static void Call(typename t_decompose::type* pObj, const T& repo) {
    const typename t_decompose::t_arg1* arg1;
    const typename t_decompose::t_arg2* arg2;
    repo.Get(arg1);
    repo.Get(arg2);
    (pObj->*memfn)(*arg1, *arg2);
  }
};

template<class T, class MemFn, MemFn memfn>
struct BoundCall<T, MemFn, memfn, 3>:
  Decompose<MemFn>
{
  typedef Decompose<MemFn> t_decompose;

  static void Call(typename t_decompose::type* pObj, const T& repo) {
    const typename t_decompose::t_arg1* arg1;
    const typename t_decompose::t_arg2* arg2;
    const typename t_decompose::t_arg2* arg3;
    repo.Get(arg1);
    repo.Get(arg2);
    repo.Get(arg3);
    (pObj->*memfn)(*arg1, *arg2, *arg3);
  }
};

template<class T, class MemFn, MemFn memfn>
struct BoundCall<T, MemFn, memfn, 4>:
  Decompose<MemFn>
{
  typedef Decompose<MemFn> t_decompose;

  static void Call(typename t_decompose::type* pObj, const T& repo) {
    const typename t_decompose::t_arg1* arg1;
    const typename t_decompose::t_arg2* arg2;
    const typename t_decompose::t_arg2* arg3;
    const typename t_decompose::t_arg2* arg4;
    repo.Get(arg1);
    repo.Get(arg2);
    repo.Get(arg3);
    repo.Get(arg4);
    (pObj->*memfn)(*arg1, *arg2, *arg3, *arg4);
  }
};