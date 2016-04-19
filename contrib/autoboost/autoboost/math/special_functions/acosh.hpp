//    autoboost asinh.hpp header file

//  (C) Copyright Eric Ford 2001 & Hubert Holin.
//  (C) Copyright John Maddock 2008.
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

// See http://www.boost.org for updates, documentation, and revision history.

#ifndef AUTOBOOST_ACOSH_HPP
#define AUTOBOOST_ACOSH_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <autoboost/config/no_tr1/cmath.hpp>
#include <autoboost/config.hpp>
#include <autoboost/math/tools/precision.hpp>
#include <autoboost/math/policies/error_handling.hpp>
#include <autoboost/math/special_functions/math_fwd.hpp>
#include <autoboost/math/special_functions/log1p.hpp>
#include <autoboost/math/constants/constants.hpp>

// This is the inverse of the hyperbolic cosine function.

namespace autoboost
{
    namespace math
    {
       namespace detail
       {
        template<typename T, typename Policy>
        inline T    acosh_imp(const T x, const Policy& pol)
        {
            AUTOBOOST_MATH_STD_USING

            if(x < 1)
            {
               return policies::raise_domain_error<T>(
                  "autoboost::math::acosh<%1%>(%1%)",
                  "acosh requires x >= 1, but got x = %1%.", x, pol);
            }
            else if    ((x - 1) >= tools::root_epsilon<T>())
            {
                if    (x > 1 / tools::root_epsilon<T>())
                {
                    // http://functions.wolfram.com/ElementaryFunctions/ArcCosh/06/01/06/01/0001/
                    // approximation by laurent series in 1/x at 0+ order from -1 to 0
                    return log(x) + constants::ln_two<T>();
                }
                else if(x < 1.5f)
                {
                   // This is just a rearrangement of the standard form below
                   // devised to minimse loss of precision when x ~ 1:
                   T y = x - 1;
                   return autoboost::math::log1p(y + sqrt(y * y + 2 * y), pol);
                }
                else
                {
                    // http://functions.wolfram.com/ElementaryFunctions/ArcCosh/02/
                    return( log( x + sqrt(x * x - 1) ) );
                }
            }
            else
            {
                // see http://functions.wolfram.com/ElementaryFunctions/ArcCosh/06/01/04/01/0001/
                T y = x - 1;

                // approximation by taylor series in y at 0 up to order 2
                T result = sqrt(2 * y) * (1 - y /12 + 3 * y * y / 160);
                return result;
            }
        }
       }

        template<typename T, typename Policy>
        inline typename tools::promote_args<T>::type acosh(T x, const Policy&)
        {
            typedef typename tools::promote_args<T>::type result_type;
            typedef typename policies::evaluation<result_type, Policy>::type value_type;
            typedef typename policies::normalise<
               Policy,
               policies::promote_float<false>,
               policies::promote_double<false>,
               policies::discrete_quantile<>,
               policies::assert_undefined<> >::type forwarding_policy;
           return policies::checked_narrowing_cast<result_type, forwarding_policy>(
              detail::acosh_imp(static_cast<value_type>(x), forwarding_policy()),
              "autoboost::math::acosh<%1%>(%1%)");
        }
        template<typename T>
        inline typename tools::promote_args<T>::type acosh(T x)
        {
           return autoboost::math::acosh(x, policies::policy<>());
        }

    }
}

#endif /* AUTOBOOST_ACOSH_HPP */


