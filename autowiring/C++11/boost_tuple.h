// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once

#include <autoboost/tuple/tuple.hpp>
#include <autoboost/tuple/tuple_comparison.hpp>

namespace std {
  template<typename... Ts>
  class tuple;

  template<typename T, typename... Ts>
  class tuple<T, Ts...> {
  public:
    tuple(void) {}
    tuple(const T& ele, const Ts&... eles):
      m_tuple(ele, eles...)
    {}
    virtual ~tuple(void){}

    tuple& operator=(const tuple<T, Ts...>& other) {
      m_tuple = other.m_tuple;
      return *this;
    }

    template<class... OtherTs>
    tuple& operator=(const tuple<OtherTs...>& other) {
      m_tuple = other;
      return *this;
    }

    bool operator==(const tuple<T, Ts...>& other) const {
      return m_tuple == other.m_tuple;
    }

    bool operator<(const tuple<T, Ts...>& other) const {
      return m_tuple < other.m_tuple;
    }

    autoboost::tuple<T, Ts...> m_tuple;
  };

  template<>
  class tuple<> {
  public:
    tuple(void) {}
    virtual ~tuple(void){}

    tuple& operator=(const tuple<>& other) {
      return *this;
    }

    bool operator==(const tuple<>& other) const {
      return true;
    }

    bool operator<(const tuple<>& other) const {
      return false;
    }

    autoboost::tuple<> m_tuple;
  };

  template<typename... Ts>
  ::std::tuple<Ts...> tie(const Ts&... val) {
    return ::std::tuple<Ts...>(val...) ;
  }

  template<int I, typename... Ts>
  auto get(const ::std::tuple<Ts...>& tup) -> decltype(autoboost::get<I>(tup.m_tuple)) {
    return autoboost::get<I>(tup.m_tuple);
  }

  template<typename... Ts>
  ::std::tuple<Ts...> make_tuple(const Ts&... ele) {
    return ::std::tuple<Ts...>(ele...);
  }

}//namespace std
