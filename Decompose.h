#pragma once

/// <summary>
/// Provides some static reflection support for member function pointers
/// </summary>
template<class MemFn>
struct Decompose;

template<class R, class W>
struct Decompose<R(W::*)()> {
  typedef void fnType();
  typedef W type;
  typedef R retType;

  template<class _Fx>
  static void Enumerate(_Fx&& fx) {}
};

template<class R, class W, class Arg1>
struct Decompose<R(W::*)(Arg1)> {
  typedef void fnType(Arg1);
  typedef W type;
  typedef R retType;

  template<class _Fx>
  static void Enumerate(_Fx&& fx) {
    fx(typeid(Arg1));
  }
};

template<class R, class W, class Arg1, class Arg2>
struct Decompose<R(W::*)(Arg1, Arg2)> {
  typedef void fnType(Arg1, Arg2);
  typedef W type;
  typedef R retType;

  template<class _Fx>
  static void Enumerate(_Fx&& fx) {
    fx(typeid(Arg1));
    fx(typeid(Arg2));
  }
};

template<class R, class W, class Arg1, class Arg2, class Arg3>
struct Decompose<R(W::*)(Arg1, Arg2, Arg3)> {
  typedef void fnType(Arg1, Arg2, Arg3);
  typedef W type;
  typedef R retType;

  template<class _Fx>
  static void Enumerate(_Fx&& fx) {
    fx(typeid(Arg1));
    fx(typeid(Arg2));
    fx(typeid(Arg3));
  }
};

template<class R, class W, class Arg1, class Arg2, class Arg3, class Arg4>
struct Decompose<R(W::*)(Arg1, Arg2, Arg3, Arg4)> {
  typedef void fnType(Arg1, Arg2, Arg3, Arg4);
  typedef W type;
  typedef R retType;

  template<class _Fx>
  static void Enumerate(_Fx&& fx) {
    fx(typeid(Arg1));
    fx(typeid(Arg2));
    fx(typeid(Arg3));
    fx(typeid(Arg4));
  }
};