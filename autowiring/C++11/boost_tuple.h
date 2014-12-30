// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

namespace std {
  template<typename... Ts>
  class tuple {
  public:
    tuple(const Ts&... ele):
      m_tuple(ele...)
    {}
    virtual ~tuple(void){}
    
    tuple& operator=(const tuple<Ts...>& other) {
      m_tuple = other.m_tuple;
      return *this;
    }

    bool operator==(const tuple<Ts...>& other) const {
      return m_tuple == other.m_tuple;
    }

    bool operator<(const tuple<Ts...>& other) const {
      return m_tuple < other.m_tuple;
    }

    AUTOWIRING_BOOST_NAME::tuple<Ts...> m_tuple;
  };
  
  template<typename... Ts>
  ::std::tuple<Ts...> tie(const Ts&... val) {
    return ::std::tuple<Ts...>(val...) ;
  }

  template<int I, typename... Ts>
  auto get(const ::std::tuple<Ts...>& tup) -> decltype(AUTOWIRING_BOOST_NAME::get<I>(tup.m_tuple)) {
    return AUTOWIRING_BOOST_NAME::get<I>(tup.m_tuple);
  }

  template<typename... Ts>
  ::std::tuple<Ts...> make_tuple(const Ts&... ele) {
    return ::std::tuple<Ts...>(ele...);
  }

}//namespace std
