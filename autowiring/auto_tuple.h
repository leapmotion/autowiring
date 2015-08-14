// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "is_any.h"
#include "index_tuple.h"
#include "sum.h"

namespace autowiring {
  /// <summary>
  /// Autowiring specialized tuple type
  /// </summary>
  template<class... Args>
  struct tuple {};

  /// <summary>
  /// Finds the specified type T in the argument pack
  /// </remarks>
  template<typename T, typename... Args>
  struct find {
    static const bool value = false;
    static const size_t index = 0;
  };
  
  template<typename T, typename Arg, typename... Args>
  struct find<T, Arg, Args...> {
    // Holds true if T is found, false otherwise
    static const bool value =
      std::is_same<T, Arg>::value ||
      find<T, Args...>::value;

    // Holds one more than the index of the found item.  In the case that multiple
    // matches are found, holds the sum of all of those values.
    static const size_t index =
      (value ? 1 + find<T, Args...>::index : 0);
  };

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

  template<int N, class... Args>
  const typename nth_type<N, Args...>::type& get(const tuple<Args...>& val) {
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
  Arg& get(tuple<Args...>& val) {
    static_assert(
      is_any<std::is_same<Arg, Args>::value...>::value,
      "Requested type is not one of any of the types held in the specified tuple"
    );

    static const size_t index = find<Arg, Args...>::index;
    static_assert(
      index <= sizeof...(Args),
      "Requested type appears multiple times in the specified tuple"
    );
    return get<index - 1>(val);
  }

  template<class Arg, class... Args>
  struct tuple<Arg, Args...>:
    tuple<Args...>,
    tuple_value<sizeof...(Args), Arg>
  {
    typedef tuple_value<sizeof...(Args), Arg> t_value;

    tuple(void) = default;
    tuple(const tuple&) = default;

    tuple(Arg&& arg, Args&&... args) :
      tuple<Args...>(std::forward<Args>(args)...),
      tuple_value<sizeof...(Args), Arg>(std::forward<Arg&&>(arg))
    {}

    template<class OtherT, class... OtherTs>
    tuple& operator=(const tuple<OtherT, OtherTs...>& rhs) {
      // Base type copy
      static_cast<tuple<Args...>&>(*this) = static_cast<const tuple<OtherTs...>&>(rhs);

      // Interior copy:
      t_value::value = static_cast<const typename tuple<OtherT, OtherTs...>::t_value&>(rhs).value;
      return *this;
    }
  };

  template<class... Args>
  tuple<Args&...> tie(Args&... args) {
    return tuple<Args&...>(args...);
  }

  template<class... Args>
  tuple<Args...> make_tuple(Args&&... args) {
    return tuple<Args...>(std::forward<Args&&>(args)...);
  }
}