#ifndef AUTOBOOST_SMART_PTR_DETAIL_SP_NOEXCEPT_HPP_INCLUDED
#define AUTOBOOST_SMART_PTR_DETAIL_SP_NOEXCEPT_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  detail/sp_noexcept.hpp
//
//  Copyright 2016 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

#include <autoboost/config.hpp>

#if defined( AUTOBOOST_MSVC ) && AUTOBOOST_MSVC >= 1700 && AUTOBOOST_MSVC < 1900

#define AUTOBOOST_SP_NOEXCEPT AUTOBOOST_NOEXCEPT_OR_NOTHROW

#else

#define AUTOBOOST_SP_NOEXCEPT AUTOBOOST_NOEXCEPT

#endif

#endif  // #ifndef AUTOBOOST_SMART_PTR_DETAIL_SP_NOEXCEPT_HPP_INCLUDED
