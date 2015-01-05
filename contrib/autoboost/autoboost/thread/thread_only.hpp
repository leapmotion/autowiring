#ifndef AUTOBOOST_THREAD_THREAD_ONLY_HPP
#define AUTOBOOST_THREAD_THREAD_ONLY_HPP

//  thread.hpp
//
//  (C) Copyright 2013 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <autoboost/thread/detail/platform.hpp>

#if defined(AUTOBOOST_THREAD_PLATFORM_WIN32)
#include <autoboost/thread/win32/thread_data.hpp>
#elif defined(AUTOBOOST_THREAD_PLATFORM_PTHREAD)
#include <autoboost/thread/pthread/thread_data.hpp>
#else
#error "Boost threads unavailable on this platform"
#endif

#include <autoboost/thread/detail/thread.hpp>
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
#include <autoboost/thread/detail/thread_interruption.hpp>
#endif
#include <autoboost/thread/v2/thread.hpp>


#endif
