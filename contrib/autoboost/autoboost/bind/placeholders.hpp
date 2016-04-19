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
//  Copyright 2015 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//
//  See http://www.boost.org/libs/bind/bind.html for documentation.
//

#include <autoboost/bind/arg.hpp>
#include <autoboost/config.hpp>

namespace autoboost
{

namespace placeholders
{

#if defined(__BORLANDC__) || defined(__GNUC__) && (__GNUC__ < 4)

inline autoboost::arg<1> _1() { return autoboost::arg<1>(); }
inline autoboost::arg<2> _2() { return autoboost::arg<2>(); }
inline autoboost::arg<3> _3() { return autoboost::arg<3>(); }
inline autoboost::arg<4> _4() { return autoboost::arg<4>(); }
inline autoboost::arg<5> _5() { return autoboost::arg<5>(); }
inline autoboost::arg<6> _6() { return autoboost::arg<6>(); }
inline autoboost::arg<7> _7() { return autoboost::arg<7>(); }
inline autoboost::arg<8> _8() { return autoboost::arg<8>(); }
inline autoboost::arg<9> _9() { return autoboost::arg<9>(); }

#else

AUTOBOOST_STATIC_CONSTEXPR autoboost::arg<1> _1;
AUTOBOOST_STATIC_CONSTEXPR autoboost::arg<2> _2;
AUTOBOOST_STATIC_CONSTEXPR autoboost::arg<3> _3;
AUTOBOOST_STATIC_CONSTEXPR autoboost::arg<4> _4;
AUTOBOOST_STATIC_CONSTEXPR autoboost::arg<5> _5;
AUTOBOOST_STATIC_CONSTEXPR autoboost::arg<6> _6;
AUTOBOOST_STATIC_CONSTEXPR autoboost::arg<7> _7;
AUTOBOOST_STATIC_CONSTEXPR autoboost::arg<8> _8;
AUTOBOOST_STATIC_CONSTEXPR autoboost::arg<9> _9;

#endif

} // namespace placeholders

} // namespace autoboost

#endif // #ifndef AUTOBOOST_BIND_PLACEHOLDERS_HPP_INCLUDED
