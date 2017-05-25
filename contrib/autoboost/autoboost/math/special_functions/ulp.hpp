//  (C) Copyright John Maddock 2015.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_MATH_SPECIAL_ULP_HPP
#define AUTOBOOST_MATH_SPECIAL_ULP_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <autoboost/math/special_functions/math_fwd.hpp>
#include <autoboost/math/policies/error_handling.hpp>
#include <autoboost/math/special_functions/fpclassify.hpp>
#include <autoboost/math/special_functions/next.hpp>

namespace autoboost{ namespace math{ namespace detail{

template <class T, class Policy>
T ulp_imp(const T& val, const Policy& pol)
{
   AUTOBOOST_MATH_STD_USING
   int expon;
   static const char* function = "ulp<%1%>(%1%)";

   int fpclass = (autoboost::math::fpclassify)(val);

   if(fpclass == (int)FP_NAN)
   {
      return policies::raise_domain_error<T>(
         function,
         "Argument must be finite, but got %1%", val, pol);
   }
   else if((fpclass == (int)FP_INFINITE) || (fabs(val) >= tools::max_value<T>()))
   {
      return (val < 0 ? -1 : 1) * policies::raise_overflow_error<T>(function, 0, pol);
   }
   else if(fpclass == FP_ZERO)
      return detail::get_smallest_value<T>();
   //
   // This code is almost the same as that for float_next, except for negative integers,
   // where we preserve the relation ulp(x) == ulp(-x) as does Java:
   //
   frexp(fabs(val), &expon);
   T diff = ldexp(T(1), expon - tools::digits<T>());
   if(diff == 0)
      diff = detail::get_smallest_value<T>();
   return diff;
}

}

template <class T, class Policy>
inline typename tools::promote_args<T>::type ulp(const T& val, const Policy& pol)
{
   typedef typename tools::promote_args<T>::type result_type;
   return detail::ulp_imp(static_cast<result_type>(val), pol);
}

template <class T>
inline typename tools::promote_args<T>::type ulp(const T& val)
{
   return ulp(val, policies::policy<>());
}


}} // namespaces

#endif // AUTOBOOST_MATH_SPECIAL_ULP_HPP

