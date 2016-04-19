//  ratio_fwd.hpp  ---------------------------------------------------------------//

//  Copyright 2008 Howard Hinnant
//  Copyright 2008 Beman Dawes
//  Copyright 2009 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

/*

This code was derived by Beman Dawes from Howard Hinnant's time2_demo prototype.
Many thanks to Howard for making his code available under the Boost license.
The original code was modified to conform to Boost conventions and to section
20.4 Compile-time rational arithmetic [ratio], of the C++ committee working
paper N2798.
See http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2798.pdf.

time2_demo contained this comment:

    Much thanks to Andrei Alexandrescu,
                   Walter Brown,
                   Peter Dimov,
                   Jeff Garland,
                   Terry Golubiewski,
                   Daniel Krugler,
                   Anthony Williams.
*/

// The way overflow is managed for ratio_less is taken from llvm/libcxx/include/ratio

#ifndef AUTOBOOST_RATIO_RATIO_FWD_HPP
#define AUTOBOOST_RATIO_RATIO_FWD_HPP

#include <autoboost/ratio/config.hpp>

#if defined(__GNUC__) && (__GNUC__ >= 4)
#pragma GCC system_header
#endif

namespace autoboost
{

//----------------------------------------------------------------------------//
//                                                                            //
//              20.6 Compile-time rational arithmetic [ratio]                 //
//                                                                            //
//----------------------------------------------------------------------------//

// ratio
template <autoboost::intmax_t N, autoboost::intmax_t D = 1> class ratio;

// ratio arithmetic
template <class R1, class R2> struct ratio_add;
template <class R1, class R2> struct ratio_subtract;
template <class R1, class R2> struct ratio_multiply;
template <class R1, class R2> struct ratio_divide;
#ifdef AUTOBOOST_RATIO_EXTENSIONS
template <class R1, class R2> struct ratio_gcd;
template <class R1, class R2> struct ratio_lcm;
template <class R> struct ratio_negate;
template <class R> struct ratio_abs;
template <class R> struct ratio_sign;
template <class R, int P> struct ratio_power;
#endif

// ratio comparison
template <class R1, class R2> struct ratio_equal;
template <class R1, class R2> struct ratio_not_equal;
template <class R1, class R2> struct ratio_less;
template <class R1, class R2> struct ratio_less_equal;
template <class R1, class R2> struct ratio_greater;
template <class R1, class R2> struct ratio_greater_equal;

// convenience SI typedefs
typedef ratio<AUTOBOOST_RATIO_INTMAX_C(1), AUTOBOOST_RATIO_INTMAX_C(1000000000000000000)> atto;
typedef ratio<AUTOBOOST_RATIO_INTMAX_C(1),    AUTOBOOST_RATIO_INTMAX_C(1000000000000000)> femto;
typedef ratio<AUTOBOOST_RATIO_INTMAX_C(1),       AUTOBOOST_RATIO_INTMAX_C(1000000000000)> pico;
typedef ratio<AUTOBOOST_RATIO_INTMAX_C(1),          AUTOBOOST_RATIO_INTMAX_C(1000000000)> nano;
typedef ratio<AUTOBOOST_RATIO_INTMAX_C(1),             AUTOBOOST_RATIO_INTMAX_C(1000000)> micro;
typedef ratio<AUTOBOOST_RATIO_INTMAX_C(1),                AUTOBOOST_RATIO_INTMAX_C(1000)> milli;
typedef ratio<AUTOBOOST_RATIO_INTMAX_C(1),                 AUTOBOOST_RATIO_INTMAX_C(100)> centi;
typedef ratio<AUTOBOOST_RATIO_INTMAX_C(1),                  AUTOBOOST_RATIO_INTMAX_C(10)> deci;
typedef ratio<                 AUTOBOOST_RATIO_INTMAX_C(10), AUTOBOOST_RATIO_INTMAX_C(1)> deca;
typedef ratio<                AUTOBOOST_RATIO_INTMAX_C(100), AUTOBOOST_RATIO_INTMAX_C(1)> hecto;
typedef ratio<               AUTOBOOST_RATIO_INTMAX_C(1000), AUTOBOOST_RATIO_INTMAX_C(1)> kilo;
typedef ratio<            AUTOBOOST_RATIO_INTMAX_C(1000000), AUTOBOOST_RATIO_INTMAX_C(1)> mega;
typedef ratio<         AUTOBOOST_RATIO_INTMAX_C(1000000000), AUTOBOOST_RATIO_INTMAX_C(1)> giga;
typedef ratio<      AUTOBOOST_RATIO_INTMAX_C(1000000000000), AUTOBOOST_RATIO_INTMAX_C(1)> tera;
typedef ratio<   AUTOBOOST_RATIO_INTMAX_C(1000000000000000), AUTOBOOST_RATIO_INTMAX_C(1)> peta;
typedef ratio<AUTOBOOST_RATIO_INTMAX_C(1000000000000000000), AUTOBOOST_RATIO_INTMAX_C(1)> exa;

#ifdef AUTOBOOST_RATIO_EXTENSIONS

#define AUTOBOOST_RATIO_1024 AUTOBOOST_RATIO_INTMAX_C(1024)

// convenience IEC typedefs
typedef ratio<                                                                                     AUTOBOOST_RATIO_1024> kibi;
typedef ratio<                                                                    AUTOBOOST_RATIO_1024*AUTOBOOST_RATIO_1024> mebi;
typedef ratio<                                                   AUTOBOOST_RATIO_1024*AUTOBOOST_RATIO_1024*AUTOBOOST_RATIO_1024> gibi;
typedef ratio<                                  AUTOBOOST_RATIO_1024*AUTOBOOST_RATIO_1024*AUTOBOOST_RATIO_1024*AUTOBOOST_RATIO_1024> tebi;
typedef ratio<                 AUTOBOOST_RATIO_1024*AUTOBOOST_RATIO_1024*AUTOBOOST_RATIO_1024*AUTOBOOST_RATIO_1024*AUTOBOOST_RATIO_1024> pebi;
typedef ratio<AUTOBOOST_RATIO_1024*AUTOBOOST_RATIO_1024*AUTOBOOST_RATIO_1024*AUTOBOOST_RATIO_1024*AUTOBOOST_RATIO_1024*AUTOBOOST_RATIO_1024> exbi;

#endif
}  // namespace autoboost


#endif  // AUTOBOOST_RATIO_HPP
