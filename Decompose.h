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
struct is_any_same<ToCheck, Head, Tail...> {
  static const bool value = std::is_same<ToCheck, Head>::value || is_any_same<ToCheck, Tail...>::value;
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
struct Decompose {};

template<class R, class W, class... Args>
struct Decompose<R(W::*)(Args...)> {
  typedef R (W::*memType)(Args...);
  typedef void fnType(Args...);
  typedef W type;
  typedef R retType;
  static const int N = sizeof...(Args);

  /// <summary>
  /// Returns an array of length N+1 of argument types on the bound type
  /// </summary>
  /// <remarks>
  /// The last entry in this list is guaranteed to be nullptr.  The list's actual size is
  /// one larger than the number of arguments, in order to allow a proper return for a zero-
  /// argument function pointer.
  /// </remarks>
  static const std::type_info* (&Enumerate(void))[N + 1] {
    static const std::type_info* ti[] = {&typeid(Args)..., nullptr};
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
    static const T rb[] = {typename T::template rebind<Args>()..., T()};
    return rb;
  }

  /// <summary>
  /// Performs a call on the specified object with the specified member function
  /// <summary>
  /// <remarks>
  /// The passed type object, the last parameter, must be a type repository.  It must support
  /// a method of the following form:
  ///
  /// template&lt;class T&gt;
  /// const T& Get();
  ///
  /// The method must return the object of the desired type, or else it must throw an exception.
  /// It is the caller's responsibility to ensure that all of the necessary types are available
  /// before this call is made, in order to prevent such an exception from being thrown.
  /// </remarks>
  template<memType memfn, class Repo>
  static void Call(W* pObj, Repo& repo)
  {
    (pObj->*memfn)(
      repo.template Cast<Args>()...
    );
  }
};
