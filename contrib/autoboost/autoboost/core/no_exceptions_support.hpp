#ifndef AUTOBOOST_CORE_NO_EXCEPTIONS_SUPPORT_HPP
#define AUTOBOOST_CORE_NO_EXCEPTIONS_SUPPORT_HPP

#if defined(_MSC_VER)
#  pragma once
#endif

//----------------------------------------------------------------------
// (C) Copyright 2004 Pavel Vozenilek.
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
// This file contains helper macros used when exception support may be
// disabled (as indicated by macro AUTOBOOST_NO_EXCEPTIONS).
//
// Before picking up these macros you may consider using RAII techniques
// to deal with exceptions - their syntax can be always the same with
// or without exception support enabled.
//----------------------------------------------------------------------

#include <autoboost/config.hpp>
#include <autoboost/detail/workaround.hpp>

#if !(defined AUTOBOOST_NO_EXCEPTIONS)
#    define AUTOBOOST_TRY { try
#    define AUTOBOOST_CATCH(x) catch(x)
#    define AUTOBOOST_RETHROW throw;
#    define AUTOBOOST_CATCH_END }
#else
#    if AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x564))
#        define AUTOBOOST_TRY { if ("")
#        define AUTOBOOST_CATCH(x) else if (!"")
#    else
#        define AUTOBOOST_TRY { if (true)
#        define AUTOBOOST_CATCH(x) else if (false)
#    endif
#    define AUTOBOOST_RETHROW
#    define AUTOBOOST_CATCH_END }
#endif


#endif
