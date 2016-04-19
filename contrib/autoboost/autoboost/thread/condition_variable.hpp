#ifndef AUTOBOOST_THREAD_CONDITION_VARIABLE_HPP
#define AUTOBOOST_THREAD_CONDITION_VARIABLE_HPP

//  condition_variable.hpp
//
//  (C) Copyright 2007 Anthony Williams
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <autoboost/thread/detail/platform.hpp>
#if defined(AUTOBOOST_THREAD_PLATFORM_WIN32)
#include <autoboost/thread/win32/condition_variable.hpp>
#elif defined(AUTOBOOST_THREAD_PLATFORM_PTHREAD)
#include <autoboost/thread/pthread/condition_variable.hpp>
#else
#error "Boost threads unavailable on this platform"
#endif

#endif
