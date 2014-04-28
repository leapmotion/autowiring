#pragma once
#include TYPE_TRAITS_HEADER
#include <typeinfo>

/// <summary>
/// Extended version of is_same
/// </summary>
/// <remarks>
/// Inherits from true_type if T is the same as any of U1...UN
/// </remarks>
template<typename... T>
struct is_any_same{};

template<typename ToCheck>
struct is_any_same<ToCheck>{
  static const bool value = false;
};

template<typename ToCheck, typename Head, typename... Tail>
struct is_any_same<ToCheck, Head, Tail...>{
  static const bool value = std::is_same<ToCheck,Head>::value || is_any_same<ToCheck,Tail...>::value;
};


struct type_info_constructable {
  type_info_constructable(const std::type_info* ti = nullptr) :
    ti(ti)
  {}

  const std::type_info* ti;

  const std::type_info* operator->(void) const { return ti; }
  operator const std::type_info*(void) const { return ti; }

  template<class T>
  struct rebind {
    operator type_info_constructable(void) const {return type_info_constructable(&typeid(T));}
  };
};

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

  /// <returns>
  /// An array of type T, parameterized by the bound function's arguments
  /// </returns>
  /// <remarks>
  /// Type T must be move-constructable.
  ///
  /// The returned array contains one more element than the arity of the decomposed member function
  /// type.  Each element in the array is initialized based on the type of the corresponding argument
  /// in the decomposed function.  Elements in the array are constructed using the "rebind" structure
  /// which must be an interior type to type T.  An instance of type rebind should be castable to the
  /// base type T, or it must be a function returning a value of type T.
  /// </remarks>
  template<class T>
  static const T(&Enumerate(void))[N + 1] {
    static const T rb[] = {T()};
    return rb;
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

  template<class T>
  static const T(&Enumerate(void))[N + 1] {
    static const T rb[] = {typename T::template rebind<t_arg1>(), T()};
    return rb;
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

  template<class Token, class Fx>
  static void Enumerate(Token& token, Fx& fx) {
    fx.template operator()<t_arg1>();
    fx.template operator()<t_arg2>();
  }

  template<class T>
  static const T(&Enumerate(void))[N + 1] {
    static const T rb[] = {typename T::template rebind<t_arg1>(), typename T::template rebind<t_arg2>(), T()};
    return rb;
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

  template<class T>
  static const T(&Enumerate(void))[N + 1] {
    static const T rb[] = {typename T::template rebind<t_arg1>(), typename T::template rebind<t_arg2>(), typename T::template rebind<t_arg3>(), T()};
    return rb;
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

  template<class T>
  static const T(&Enumerate(void))[N + 1] {
    static const T rb[] = {typename T::template rebind<t_arg1>(), typename T::template rebind<t_arg2>(), typename T::template rebind<t_arg3>(), typename T::template rebind<t_arg4>(), T()};
    return rb;
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
  typedef typename Decompose<MemFn>::t_arg1 t_arg1;

  static void Call(typename t_decompose::type* pObj, T& repo) {
    (pObj->*memfn)(
      repo.template Cast<t_arg1>()
    );
  }
};

template<class T, class MemFn, MemFn memfn>
struct BoundCall<T, MemFn, memfn, 2>:
  Decompose<MemFn>
{
  typedef Decompose<MemFn> t_decompose;
  typedef typename Decompose<MemFn>::t_arg1 t_arg1;
  typedef typename Decompose<MemFn>::t_arg2 t_arg2;

  static void Call(typename t_decompose::type* pObj, T& repo) {
    (pObj->*memfn)(
      repo.template Cast<t_arg1>(),
      repo.template Cast<t_arg2>()
    );
  }
};

template<class T, class MemFn, MemFn memfn>
struct BoundCall<T, MemFn, memfn, 3>:
  Decompose<MemFn>
{
  typedef Decompose<MemFn> t_decompose;
  typedef typename Decompose<MemFn>::t_arg1 t_arg1;
  typedef typename Decompose<MemFn>::t_arg2 t_arg2;
  typedef typename Decompose<MemFn>::t_arg3 t_arg3;

  static void Call(typename t_decompose::type* pObj, T& repo) {
    (pObj->*memfn)(
      repo.template Cast<t_arg1>(),
      repo.template Cast<t_arg2>(),
      repo.template Cast<t_arg3>()
    );
  }
};

template<class T, class MemFn, MemFn memfn>
struct BoundCall<T, MemFn, memfn, 4>:
  Decompose<MemFn>
{
  typedef Decompose<MemFn> t_decompose;
  typedef typename Decompose<MemFn>::t_arg1 t_arg1;
  typedef typename Decompose<MemFn>::t_arg2 t_arg2;
  typedef typename Decompose<MemFn>::t_arg3 t_arg3;
  typedef typename Decompose<MemFn>::t_arg4 t_arg4;

  static void Call(typename t_decompose::type* pObj, T& repo) {
    (pObj->*memfn)(
      repo.template Cast<t_arg1>(),
      repo.template Cast<t_arg2>(),
      repo.template Cast<t_arg3>(),
      repo.template Cast<t_arg4>()
    );
  }
};