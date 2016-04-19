// Copyright (C) 2013 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// 2013/10 Vicente J. Botet Escriba
//   Creation.

#ifndef AUTOBOOST_CSBL_VECTOR_HPP
#define AUTOBOOST_CSBL_VECTOR_HPP

#include <autoboost/config.hpp>

#if defined AUTOBOOST_THREAD_USES_AUTOBOOST_VECTOR || defined AUTOBOOST_NO_CXX11_RVALUE_REFERENCES || defined AUTOBOOST_MSVC
#ifndef AUTOBOOST_THREAD_USES_AUTOBOOST_VECTOR
#define AUTOBOOST_THREAD_USES_AUTOBOOST_VECTOR
#endif
#include <autoboost/container/vector.hpp>
#else
#include <vector>
#endif

namespace autoboost
{
  namespace csbl
  {
#if defined AUTOBOOST_THREAD_USES_AUTOBOOST_VECTOR
    using ::autoboost::container::vector;
#else
    using ::std::vector;
#endif

  }
}
#endif // header
