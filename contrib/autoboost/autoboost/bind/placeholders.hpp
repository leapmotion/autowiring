#ifndef AUTOBOOST_BIND_PLACEHOLDERS_HPP_INCLUDED
#define AUTOBOOST_BIND_PLACEHOLDERS_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  bind/placeholders.hpp - _N definitions
//
//  Copyright (c) 2002 Peter Dimov and Multi Media Ltd.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/bind/bind.html for documentation.
//

#include <autoboost/bind/arg.hpp>
#include <autoboost/config.hpp>

namespace
{

#if defined(__BORLANDC__) || defined(__GNUC__) && (__GNUC__ < 4)

static inline autoboost::arg<1> _1() { return autoboost::arg<1>(); }
static inline autoboost::arg<2> _2() { return autoboost::arg<2>(); }
static inline autoboost::arg<3> _3() { return autoboost::arg<3>(); }
static inline autoboost::arg<4> _4() { return autoboost::arg<4>(); }
static inline autoboost::arg<5> _5() { return autoboost::arg<5>(); }
static inline autoboost::arg<6> _6() { return autoboost::arg<6>(); }
static inline autoboost::arg<7> _7() { return autoboost::arg<7>(); }
static inline autoboost::arg<8> _8() { return autoboost::arg<8>(); }
static inline autoboost::arg<9> _9() { return autoboost::arg<9>(); }

#elif defined(AUTOBOOST_MSVC) || (defined(__DECCXX_VER) && __DECCXX_VER <= 60590031) || defined(__MWERKS__) || \
    defined(__GNUC__) && (__GNUC__ == 4 && __GNUC_MINOR__ < 2)

static autoboost::arg<1> _1;
static autoboost::arg<2> _2;
static autoboost::arg<3> _3;
static autoboost::arg<4> _4;
static autoboost::arg<5> _5;
static autoboost::arg<6> _6;
static autoboost::arg<7> _7;
static autoboost::arg<8> _8;
static autoboost::arg<9> _9;

#else


#endif

} // unnamed namespace

#endif // #ifndef AUTOBOOST_BIND_PLACEHOLDERS_HPP_INCLUDED
