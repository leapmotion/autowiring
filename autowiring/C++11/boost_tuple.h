// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <boost/tuple/tuple.hpp>

namespace std {
  template<typename... Ts>
  class tuple {
  public:
    tuple(const Ts&... ele):
      m_tuple(ele...)
    {}
    virtual ~tuple(void){}

    bool operator==(const tuple<Ts...>& other){
      return m_tuple == other.m_tuple;
    }

    bool operator<(const tuple<Ts...>& other){
      return m_tuple < other.m_tuple;
    }

    boost::tuple<Ts...> m_tuple;
  };

  template<int I, typename... Ts>
  auto get(const ::std::tuple<Ts...>& tup) -> decltype(boost::get<I>(tup.m_tuple)) {
    return boost::get<I>(tup.m_tuple);
  }

  template<typename... Ts>
  ::std::tuple<Ts...> make_tuple(const Ts&... ele) {
    return ::std::tuple<Ts...>(ele...);
  }

}//namespace std
