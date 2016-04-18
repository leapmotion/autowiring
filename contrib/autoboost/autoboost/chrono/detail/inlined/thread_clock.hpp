//  autoboost thread_clock.cpp  -----------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  See http://www.boost.org/libs/chrono for documentation.

//--------------------------------------------------------------------------------------//
#ifndef AUTOBOOST_CHRONO_DETAIL_INLINED_THREAD_CLOCK_HPP
#define AUTOBOOST_CHRONO_DETAIL_INLINED_THREAD_CLOCK_HPP

#include <autoboost/chrono/config.hpp>
#include <autoboost/version.hpp>
#if defined(AUTOBOOST_CHRONO_HAS_THREAD_CLOCK)
#include <autoboost/chrono/thread_clock.hpp>
#include <autoboost/throw_exception.hpp>
#include <autoboost/system/system_error.hpp>
#include <autoboost/throw_exception.hpp>
#include <autoboost/chrono/detail/system.hpp>

//----------------------------------------------------------------------------//
//                                Windows                                     //
//----------------------------------------------------------------------------//
#if defined(AUTOBOOST_CHRONO_WINDOWS_API)
#include <autoboost/chrono/detail/inlined/win/thread_clock.hpp>

//----------------------------------------------------------------------------//
//                                 Mac                                        //
//----------------------------------------------------------------------------//
#elif defined(AUTOBOOST_CHRONO_MAC_API)
#include <autoboost/chrono/detail/inlined/mac/thread_clock.hpp>

//----------------------------------------------------------------------------//
//                                POSIX                                     //
//----------------------------------------------------------------------------//
#elif defined(AUTOBOOST_CHRONO_POSIX_API)
#include <autoboost/chrono/detail/inlined/posix/thread_clock.hpp>

#endif  // POSIX

#endif
#endif
