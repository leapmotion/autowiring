//  (C) Copyright John Maddock 2006.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_MATH_POWM1
#define AUTOBOOST_MATH_POWM1

#ifdef _MSC_VER
#pragma once
#endif

#include <autoboost/math/special_functions/math_fwd.hpp>
#include <autoboost/math/special_functions/log1p.hpp>
#include <autoboost/math/special_functions/expm1.hpp>
#include <autoboost/math/special_functions/trunc.hpp>
#include <autoboost/assert.hpp>

namespace autoboost{ namespace math{ namespace detail{

template <class T, class Policy>
inline T powm1_imp(const T x, const T y, const Policy& pol)
{
   AUTOBOOST_MATH_STD_USING
   static const char* function = "autoboost::math::powm1<%1%>(%1%, %1%)";

   if (x > 0)
   {
      if ((fabs(y * (x - 1)) < 0.5) || (fabs(y) < 0.2))
      {
         // We don't have any good/quick approximation for log(x) * y
         // so just try it and see:
         T l = y * log(x);
         if (l < 0.5)
            return autoboost::math::expm1(l);
         if (l > autoboost::math::tools::log_max_value<T>())
            return autoboost::math::policies::raise_overflow_error<T>(function, 0, pol);
         // fall through....
      }
   }
   else
   {
      // y had better be an integer:
      if (autoboost::math::trunc(y) != y)
         return autoboost::math::policies::raise_domain_error<T>(function, "For non-integral exponent, expected base > 0 but got %1%", x, pol);
      if (autoboost::math::trunc(y / 2) == y / 2)
         return powm1_imp(T(-x), y, pol);
   }
   return pow(x, y) - 1;
}

} // detail

template <class T1, class T2>
inline typename tools::promote_args<T1, T2>::type
   powm1(const T1 a, const T2 z)
{
   typedef typename tools::promote_args<T1, T2>::type result_type;
   return detail::powm1_imp(static_cast<result_type>(a), static_cast<result_type>(z), policies::policy<>());
}

template <class T1, class T2, class Policy>
inline typename tools::promote_args<T1, T2>::type
   powm1(const T1 a, const T2 z, const Policy& pol)
{
   typedef typename tools::promote_args<T1, T2>::type result_type;
   return detail::powm1_imp(static_cast<result_type>(a), static_cast<result_type>(z), pol);
}

} // namespace math
} // namespace autoboost

#endif // AUTOBOOST_MATH_POWM1





