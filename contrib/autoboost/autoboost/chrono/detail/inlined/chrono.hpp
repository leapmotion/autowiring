//  chrono.cpp  --------------------------------------------------------------//

//  Copyright Beman Dawes 2008
//  Copyright Vicente J. Botet Escriba 2009

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#ifndef AUTOBOOST_CHRONO_DETAIL_INLINED_CHRONO_HPP
#define AUTOBOOST_CHRONO_DETAIL_INLINED_CHRONO_HPP

#include <autoboost/version.hpp>
#include <autoboost/chrono/chrono.hpp>
#if defined AUTOBOOST_CHRONO_PROVIDE_HYBRID_ERROR_HANDLING
#include <autoboost/system/system_error.hpp>
#endif
#include <autoboost/throw_exception.hpp>
#include <autoboost/chrono/detail/system.hpp>

//----------------------------------------------------------------------------//
//                                                                            //
//                     Platform-specific Implementations                      //
//                                                                            //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
//                                Windows                                     //
//----------------------------------------------------------------------------//
#if defined(AUTOBOOST_CHRONO_WINDOWS_API)
#include <autoboost/chrono/detail/inlined/win/chrono.hpp>

//----------------------------------------------------------------------------//
//                                 Mac                                        //
//----------------------------------------------------------------------------//
#elif defined(AUTOBOOST_CHRONO_MAC_API)
#include <autoboost/chrono/detail/inlined/mac/chrono.hpp>

//----------------------------------------------------------------------------//
//                                POSIX                                     //
//----------------------------------------------------------------------------//
#elif defined(AUTOBOOST_CHRONO_POSIX_API)
#include <autoboost/chrono/detail/inlined/posix/chrono.hpp>

#endif  // POSIX

#endif
