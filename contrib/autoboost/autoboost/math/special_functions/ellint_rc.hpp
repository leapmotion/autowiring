//  Copyright (c) 2006 Xiaogang Zhang, 2015 John Maddock
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//  History:
//  XZ wrote the original of this file as part of the Google
//  Summer of Code 2006.  JM modified it to fit into the
//  Boost.Math conceptual framework better, and to correctly
//  handle the y < 0 case.
//  Updated 2015 to use Carlson's latest methods.
//

#ifndef AUTOBOOST_MATH_ELLINT_RC_HPP
#define AUTOBOOST_MATH_ELLINT_RC_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <autoboost/math/policies/error_handling.hpp>
#include <autoboost/math/tools/config.hpp>
#include <autoboost/math/special_functions/math_fwd.hpp>
#include <autoboost/math/special_functions/log1p.hpp>
#include <autoboost/math/constants/constants.hpp>
#include <iostream>

// Carlson's degenerate elliptic integral
// R_C(x, y) = R_F(x, y, y) = 0.5 * \int_{0}^{\infty} (t+x)^{-1/2} (t+y)^{-1} dt
// Carlson, Numerische Mathematik, vol 33, 1 (1979)

namespace autoboost { namespace math { namespace detail{

template <typename T, typename Policy>
T ellint_rc_imp(T x, T y, const Policy& pol)
{
    AUTOBOOST_MATH_STD_USING

    static const char* function = "autoboost::math::ellint_rc<%1%>(%1%,%1%)";

    if(x < 0)
    {
       return policies::raise_domain_error<T>(function,
            "Argument x must be non-negative but got %1%", x, pol);
    }
    if(y == 0)
    {
       return policies::raise_domain_error<T>(function,
            "Argument y must not be zero but got %1%", y, pol);
    }

    // for y < 0, the integral is singular, return Cauchy principal value
    T prefix, result;
    if(y < 0)
    {
        prefix = sqrt(x / (x - y));
        x = x - y;
        y = -y;
    }
    else
       prefix = 1;

    if(x == 0)
    {
       result = constants::half_pi<T>() / sqrt(y);
    }
    else if(x == y)
    {
       result = 1 / sqrt(x);
    }
    else if(y > x)
    {
       result = atan(sqrt((y - x) / x)) / sqrt(y - x);
    }
    else
    {
       if(y / x > 0.5)
       {
          T arg = sqrt((x - y) / x);
          result = (autoboost::math::log1p(arg) - autoboost::math::log1p(-arg)) / (2 * sqrt(x - y));
       }
       else
       {
          result = log((sqrt(x) + sqrt(x - y)) / sqrt(y)) / sqrt(x - y);
       }
    }
    return prefix * result;
}

} // namespace detail

template <class T1, class T2, class Policy>
inline typename tools::promote_args<T1, T2>::type
   ellint_rc(T1 x, T2 y, const Policy& pol)
{
   typedef typename tools::promote_args<T1, T2>::type result_type;
   typedef typename policies::evaluation<result_type, Policy>::type value_type;
   return policies::checked_narrowing_cast<result_type, Policy>(
      detail::ellint_rc_imp(
         static_cast<value_type>(x),
         static_cast<value_type>(y), pol), "autoboost::math::ellint_rc<%1%>(%1%,%1%)");
}

template <class T1, class T2>
inline typename tools::promote_args<T1, T2>::type
   ellint_rc(T1 x, T2 y)
{
   return ellint_rc(x, y, policies::policy<>());
}

}} // namespaces

#endif // AUTOBOOST_MATH_ELLINT_RC_HPP

