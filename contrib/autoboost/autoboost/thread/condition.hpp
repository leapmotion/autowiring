#ifndef AUTOBOOST_THREAD_CONDITION_HPP
#define AUTOBOOST_THREAD_CONDITION_HPP
//  (C) Copyright 2007 Anthony Williams
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <autoboost/thread/detail/config.hpp>

#if defined AUTOBOOST_THREAD_PROVIDES_CONDITION

#include <autoboost/thread/condition_variable.hpp>

namespace autoboost
{
    typedef condition_variable_any condition;
}

#endif
#endif
