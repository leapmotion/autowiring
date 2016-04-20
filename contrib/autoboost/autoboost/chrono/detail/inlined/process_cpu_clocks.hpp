//  autoboost process_cpu_clocks.cpp  -----------------------------------------------------------//

//  Copyright 2009-2010 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  See http://www.boost.org/libs/chrono for documentation.

//--------------------------------------------------------------------------------------//
#ifndef AUTOBOOST_CHRONO_DETAIL_INLINED_PROCESS_CPU_CLOCKS_HPP
#define AUTOBOOST_CHRONO_DETAIL_INLINED_PROCESS_CPU_CLOCKS_HPP


#include <autoboost/chrono/config.hpp>
#if defined(AUTOBOOST_CHRONO_HAS_PROCESS_CLOCKS)

#include <autoboost/version.hpp>
#include <autoboost/chrono/process_cpu_clocks.hpp>
#include <autoboost/throw_exception.hpp>
#include <autoboost/system/system_error.hpp>

//----------------------------------------------------------------------------//
//                                Windows                                     //
//----------------------------------------------------------------------------//
#if defined(AUTOBOOST_CHRONO_WINDOWS_API)
#include <autoboost/chrono/detail/inlined/win/process_cpu_clocks.hpp>

//----------------------------------------------------------------------------//
//                                 Mac                                        //
//----------------------------------------------------------------------------//
#elif defined(AUTOBOOST_CHRONO_MAC_API)
#include <autoboost/chrono/detail/inlined/mac/process_cpu_clocks.hpp>

//----------------------------------------------------------------------------//
//                                POSIX                                     //
//----------------------------------------------------------------------------//
#elif defined(AUTOBOOST_CHRONO_POSIX_API)
#include <autoboost/chrono/detail/inlined/posix/process_cpu_clocks.hpp>

#endif  // POSIX

#endif

#endif
