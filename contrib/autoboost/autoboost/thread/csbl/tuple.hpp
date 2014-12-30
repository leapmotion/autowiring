// Copyright (C) 2013 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// 2013/10 Vicente J. Botet Escriba
//   Creation.

#ifndef AUTOBOOST_CSBL_TUPLE_HPP
#define AUTOBOOST_CSBL_TUPLE_HPP

#include <autoboost/config.hpp>

#if defined AUTOBOOST_THREAD_USES_AUTOBOOST_TUPLE || defined AUTOBOOST_NO_CXX11_HDR_TUPLE || defined AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
#include <autoboost/tuple/tuple.hpp>
#ifndef AUTOBOOST_THREAD_USES_AUTOBOOST_TUPLE
#define AUTOBOOST_THREAD_USES_AUTOBOOST_TUPLE
#endif

#else
#include <tuple>
#endif

namespace autoboost
{
  namespace csbl
  {
#if defined AUTOBOOST_THREAD_USES_AUTOBOOST_TUPLE
    using ::autoboost::tuple;
    using ::autoboost::get;
    using ::autoboost::make_tuple;
    //using ::autoboost::tuple_size;
#else
    // 20.4.2, class template tuple:
    using ::std::tuple;
    using ::std::get;
    using ::std::make_tuple;
    using ::std::tuple_size;
    // 20.4.2.4, tuple creation functions:
    // 20.4.2.5, tuple helper classes:
    // 20.4.2.6, element access:
    // 20.4.2.7, relational operators:
    // 20.4.2.8, allocator-related traits
    // 20.4.2.9, specialized algorithms:
#endif

  }
}
#endif // header
