// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once

namespace autowiring {
  /// <summary>
  /// Autowiring specialized tuple type
  /// </summary>
  template<class... Args>
  struct tuple {};

  template<int N, class... Args>
  struct nth_type;

  template<class Head, class... Tail>
  struct nth_type<0, Head, Tail...> {
    typedef Head type;
  };

  template<int N, class Head, class... Tail>
  struct nth_type<N, Head, Tail...>:
    nth_type<N - 1, Tail...>
  {};

  template<int N, class T>
  struct tuple_value {
    tuple_value(void) = default;

    tuple_value(T&& value) :
      value(std::forward<T&&>(value))
    {}

    T value;
  };

  template<int N, class... Args>
  typename nth_type<N, Args...>::type& get(tuple<Args...>& val) {
    static_assert(N < sizeof...(Args), "Requested tuple index is out of bounds");
    return
      static_cast<
        tuple_value<
          sizeof...(Args) - N - 1,
          typename nth_type<N, Args...>::type
        >&
      >(val).value;
  }

  template<class Arg, class... Args>
  struct tuple<Arg, Args...>:
    tuple<Args...>,
    tuple_value<sizeof...(Args), Arg>
  {
    tuple(void) = default;

    template<class T, class... Ts>
    tuple(T&& arg, Ts&&... args) :
      tuple_value<sizeof...(Ts), Arg>(std::forward<T&&>(arg)),
      tuple<Args...>(std::forward<Ts&&>(args)...)
    {}
  };
}