//  Copyright John Maddock 2007.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_MATH_ROUND_HPP
#define AUTOBOOST_MATH_ROUND_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <autoboost/math/tools/config.hpp>
#include <autoboost/math/policies/error_handling.hpp>
#include <autoboost/math/special_functions/math_fwd.hpp>
#include <autoboost/math/special_functions/fpclassify.hpp>

namespace autoboost{ namespace math{

namespace detail{

template <class T, class Policy>
inline typename tools::promote_args<T>::type round(const T& v, const Policy& pol, const mpl::false_)
{
   AUTOBOOST_MATH_STD_USING
      typedef typename tools::promote_args<T>::type result_type;
   if(!(autoboost::math::isfinite)(v))
      return policies::raise_rounding_error("autoboost::math::round<%1%>(%1%)", 0, static_cast<result_type>(v), static_cast<result_type>(v), pol);
   //
   // The logic here is rather convoluted, but avoids a number of traps,
   // see discussion here https://github.com/autoboostorg/math/pull/8
   //
   if (-0.5 < v && v < 0.5)
   {
      // special case to avoid rounding error on the direct
      // predecessor of +0.5 resp. the direct successor of -0.5 in
      // IEEE floating point types
      return 0;
   }
   else if (v > 0)
   {
      // subtract v from ceil(v) first in order to avoid rounding
      // errors on largest representable integer numbers
      result_type c(ceil(v));
      return 0.5 < c - v ? c - 1 : c;
   }
   else
   {
      // see former branch
      result_type f(floor(v));
      return 0.5 < v - f ? f + 1 : f;
   }
}
template <class T, class Policy>
inline typename tools::promote_args<T>::type round(const T& v, const Policy&, const mpl::true_)
{
   return v;
}

} // namespace detail

template <class T, class Policy>
inline typename tools::promote_args<T>::type round(const T& v, const Policy& pol)
{
   return detail::round(v, pol, mpl::bool_<detail::is_integer_for_rounding<T>::value>());
}
template <class T>
inline typename tools::promote_args<T>::type round(const T& v)
{
   return round(v, policies::policy<>());
}
//
// The following functions will not compile unless T has an
// implicit convertion to the integer types.  For user-defined
// number types this will likely not be the case.  In that case
// these functions should either be specialized for the UDT in
// question, or else overloads should be placed in the same
// namespace as the UDT: these will then be found via argument
// dependent lookup.  See our concept archetypes for examples.
//
template <class T, class Policy>
inline int iround(const T& v, const Policy& pol)
{
   AUTOBOOST_MATH_STD_USING
   T r = autoboost::math::round(v, pol);
   if((r > (std::numeric_limits<int>::max)()) || (r < (std::numeric_limits<int>::min)()))
      return static_cast<int>(policies::raise_rounding_error("autoboost::math::iround<%1%>(%1%)", 0, v, 0, pol));
   return static_cast<int>(r);
}
template <class T>
inline int iround(const T& v)
{
   return iround(v, policies::policy<>());
}

template <class T, class Policy>
inline long lround(const T& v, const Policy& pol)
{
   AUTOBOOST_MATH_STD_USING
   T r = autoboost::math::round(v, pol);
   if((r > (std::numeric_limits<long>::max)()) || (r < (std::numeric_limits<long>::min)()))
      return static_cast<long int>(policies::raise_rounding_error("autoboost::math::lround<%1%>(%1%)", 0, v, 0L, pol));
   return static_cast<long int>(r);
}
template <class T>
inline long lround(const T& v)
{
   return lround(v, policies::policy<>());
}

#ifdef AUTOBOOST_HAS_LONG_LONG

template <class T, class Policy>
inline autoboost::long_long_type llround(const T& v, const Policy& pol)
{
   AUTOBOOST_MATH_STD_USING
   T r = autoboost::math::round(v, pol);
   if((r > (std::numeric_limits<autoboost::long_long_type>::max)()) || (r < (std::numeric_limits<autoboost::long_long_type>::min)()))
      return static_cast<autoboost::long_long_type>(policies::raise_rounding_error("autoboost::math::llround<%1%>(%1%)", 0, v, static_cast<autoboost::long_long_type>(0), pol));
   return static_cast<autoboost::long_long_type>(r);
}
template <class T>
inline autoboost::long_long_type llround(const T& v)
{
   return llround(v, policies::policy<>());
}

#endif

}} // namespaces

#endif // AUTOBOOST_MATH_ROUND_HPP
