//  (C) Copyright 2013,2014 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_THREAD_EXECUTORS_WORK_HPP
#define AUTOBOOST_THREAD_EXECUTORS_WORK_HPP

#include <autoboost/thread/detail/config.hpp>
#include <autoboost/thread/detail/nullary_function.hpp>
#include <autoboost/thread/csbl/functional.hpp>

namespace autoboost
{
  namespace executors
  {
    typedef detail::nullary_function<void()> work;

#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
    typedef detail::nullary_function<void()> work_pq;
    //typedef csbl::function<void()> work_pq;
#else
    typedef csbl::function<void()> work_pq;
#endif
  }
} // namespace autoboost


#endif //  AUTOBOOST_THREAD_EXECUTORS_WORK_HPP
