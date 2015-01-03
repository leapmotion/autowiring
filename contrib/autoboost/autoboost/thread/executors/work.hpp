//  (C) Copyright 2013,2014 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_THREAD_EXECUTORS_WORK_HPP
#define AUTOBOOST_THREAD_EXECUTORS_WORK_HPP

#include <autoboost/thread/detail/config.hpp>

#if ! defined AUTOBOOST_THREAD_EXECUTORS_WORK_ACCEPTS_MOVABLE \
 && ! defined AUTOBOOST_THREAD_EXECUTORS_WORK_DONT_ACCEPT_MOVABLE
#define AUTOBOOST_THREAD_EXECUTORS_WORK_ACCEPTS_MOVABLE
//#define AUTOBOOST_THREAD_EXECUTORS_WORK_DONT_ACCEPT_MOVABLE
#endif

#if defined AUTOBOOST_THREAD_EXECUTORS_WORK_ACCEPTS_MOVABLE

#include <autoboost/thread/detail/nullary_function.hpp>

namespace autoboost
{
  namespace executors
  {
    typedef detail::nullary_function<void()> work;
  }
} // namespace autoboost

#else
#include <autoboost/thread/csbl/functional.hpp>

namespace autoboost
{
  namespace executors
  {
    typedef csbl::function<void()> work;
  }
} // namespace autoboost

#endif

#endif //  AUTOBOOST_THREAD_EXECUTORS_WORK_HPP
