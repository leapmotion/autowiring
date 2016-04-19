// Copyright (C) 2013 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// 2013/10 Vicente J. Botet Escriba
//   Creation.

#ifndef AUTOBOOST_CSBL_LIST_HPP
#define AUTOBOOST_CSBL_LIST_HPP

#include <autoboost/config.hpp>

#if defined AUTOBOOST_THREAD_USES_AUTOBOOST_LIST || defined AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
#ifndef AUTOBOOST_THREAD_USES_AUTOBOOST_LIST
#define AUTOBOOST_THREAD_USES_AUTOBOOST_LIST
#endif
#include <autoboost/container/list.hpp>
#else
#include <list>
#endif

namespace autoboost
{
  namespace csbl
  {
#if defined AUTOBOOST_THREAD_USES_AUTOBOOST_LIST
    using ::autoboost::container::list;
#else
    using ::std::list;
#endif

  }
}
#endif // header
