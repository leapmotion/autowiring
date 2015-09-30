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

  template<typename T>
  struct is_tuple {
    static const bool value = false;
  };

  template<typename... Args>
  struct is_tuple<tuple<Args...>> {
    static const bool value = true;
  };

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

  template<int N, class... Args>
  typename nth_type<N, Args...>::type& get(tuple<Args...>& val) {
    static_assert(N < sizeof...(Args), "Requested tuple index is out of bounds");
    return val.get(std::integral_constant<int, sizeof...(Args) - N - 1>{});
  }

  template<int N, class... Args>
  const typename nth_type<N, Args...>::type& get(const tuple<Args...>& val) {
    static_assert(N < sizeof...(Args), "Requested tuple index is out of bounds");
    return val.get(std::integral_constant<int, sizeof...(Args) - N - 1>{});
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

  template<typename T>
  T&& transfer(tuple<T>&& rhs) {
    return std::move(rhs.val);
  }

  template<typename T>
  typename std::enable_if<!is_tuple<T>::value, T&&>::type
  transfer(typename std::remove_reference<T>::type& rhs)
  {
    return static_cast<T&&>(rhs);
  }

  template<class Arg>
  struct tuple<Arg>
  {
  public:
    tuple(void) = default;
    tuple(const tuple& rhs) :
      val(rhs.val)
    {}
    tuple(tuple&& rhs) :
      val(std::forward<Arg&&>(rhs.val))
    {}

    template<typename T>
    explicit tuple(T&& rhs) :
      val(transfer<T&&>(rhs))
    {}

    Arg val;

    Arg& get(std::integral_constant<int, 0>&&) { return val; }
    const Arg& get(std::integral_constant<int, 0>&&) const { return val; }

    tuple& operator=(const tuple& rhs) {
      val = rhs.val;
      return *this;
    }

    template<class OtherT>
    tuple& operator=(const tuple<OtherT>& rhs) {
      val = rhs.val;
      return *this;
    }

    tuple& operator=(tuple&& rhs) {
      std::swap(val, rhs.val);
      return *this;
    }

    template<class OtherT>
    tuple& operator=(tuple<OtherT>&& rhs) {
      std::swap(val, rhs.val);
      return *this;
    }
  };

  template<class Arg, class ArgNext, class... Args>
  struct tuple<Arg, ArgNext, Args...> :
    tuple<ArgNext, Args...>
  {
    typedef tuple<ArgNext, Args...> t_base;

    tuple(void) = default;
    tuple(const tuple& rhs) :
      t_base{ static_cast<const t_base&>(rhs) },
      val(rhs.val)
    {}

    tuple(tuple&& rhs) :
      t_base{ std::forward<t_base&&>(rhs) },
      val(std::forward<Arg&&>(rhs.val))
    {}

    template<typename FnArg1, typename FnArg2, typename... FnArgs>
    tuple(
      FnArg1&& arg1,
      FnArg2&& arg2,
      FnArgs&&... args
    ) :
      t_base{ std::forward<FnArg2&&>(arg2), std::forward<FnArgs&&>(args)... },
      val(std::forward<FnArg1&&>(arg1))
    {}

    tuple& operator=(const tuple& rhs) {
      // Copy base then ourselves
      static_cast<t_base&>(*this) = static_cast<const t_base&>(rhs);
      val = rhs.val;
      return *this;
    }

    template<class OtherT, class... OtherTs>
    tuple& operator=(const tuple<OtherT, OtherTs...>& rhs) {
      // Copy base then ourselves
      static_cast<t_base&>(*this) = static_cast<const tuple<OtherTs...>&>(rhs);
      val = rhs.val;
      return *this;
    }

    tuple& operator=(tuple&& rhs) {
      // Move base then ourselves
      static_cast<t_base&>(*this) = static_cast<t_base&&>(rhs);
      val = std::move(rhs.val);
      return *this;
    }

    template<class OtherT, class... OtherTs>
    tuple& operator=(tuple<OtherT, OtherTs...>&& rhs) {
      // Move base then ourselves
      static_cast<t_base&>(*this) = static_cast<const tuple<OtherTs...>&&>(rhs);
      val = std::move(rhs.val);
      return *this;
    }

    using t_base::get;
    Arg& get(std::integral_constant<int, 1 + sizeof...(Args)>&&) { return val; }
    const Arg& get(std::integral_constant<int, 1 + sizeof...(Args)>&&) const { return val; }

    Arg val;
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
