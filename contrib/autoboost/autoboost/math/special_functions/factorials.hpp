//  Copyright John Maddock 2006, 2010.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_MATH_SP_FACTORIALS_HPP
#define AUTOBOOST_MATH_SP_FACTORIALS_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <autoboost/math/special_functions/math_fwd.hpp>
#include <autoboost/math/special_functions/gamma.hpp>
#include <autoboost/math/special_functions/detail/unchecked_factorial.hpp>
#include <autoboost/array.hpp>
#ifdef AUTOBOOST_MSVC
#pragma warning(push) // Temporary until lexical cast fixed.
#pragma warning(disable: 4127 4701)
#endif
#ifdef AUTOBOOST_MSVC
#pragma warning(pop)
#endif
#include <autoboost/config/no_tr1/cmath.hpp>

namespace autoboost { namespace math
{

template <class T, class Policy>
inline T factorial(unsigned i, const Policy& pol)
{
   AUTOBOOST_STATIC_ASSERT(!autoboost::is_integral<T>::value);
   // factorial<unsigned int>(n) is not implemented
   // because it would overflow integral type T for too small n
   // to be useful. Use instead a floating-point type,
   // and convert to an unsigned type if essential, for example:
   // unsigned int nfac = static_cast<unsigned int>(factorial<double>(n));
   // See factorial documentation for more detail.

   AUTOBOOST_MATH_STD_USING // Aid ADL for floor.

   if(i <= max_factorial<T>::value)
      return unchecked_factorial<T>(i);
   T result = autoboost::math::tgamma(static_cast<T>(i+1), pol);
   if(result > tools::max_value<T>())
      return result; // Overflowed value! (But tgamma will have signalled the error already).
   return floor(result + 0.5f);
}

template <class T>
inline T factorial(unsigned i)
{
   return factorial<T>(i, policies::policy<>());
}
/*
// Can't have these in a policy enabled world?
template<>
inline float factorial<float>(unsigned i)
{
   if(i <= max_factorial<float>::value)
      return unchecked_factorial<float>(i);
   return tools::overflow_error<float>(AUTOBOOST_CURRENT_FUNCTION);
}

template<>
inline double factorial<double>(unsigned i)
{
   if(i <= max_factorial<double>::value)
      return unchecked_factorial<double>(i);
   return tools::overflow_error<double>(AUTOBOOST_CURRENT_FUNCTION);
}
*/
template <class T, class Policy>
T double_factorial(unsigned i, const Policy& pol)
{
   AUTOBOOST_STATIC_ASSERT(!autoboost::is_integral<T>::value);
   AUTOBOOST_MATH_STD_USING  // ADL lookup of std names
   if(i & 1)
   {
      // odd i:
      if(i < max_factorial<T>::value)
      {
         unsigned n = (i - 1) / 2;
         return ceil(unchecked_factorial<T>(i) / (ldexp(T(1), (int)n) * unchecked_factorial<T>(n)) - 0.5f);
      }
      //
      // Fallthrough: i is too large to use table lookup, try the
      // gamma function instead.
      //
      T result = autoboost::math::tgamma(static_cast<T>(i) / 2 + 1, pol) / sqrt(constants::pi<T>());
      if(ldexp(tools::max_value<T>(), -static_cast<int>(i+1) / 2) > result)
         return ceil(result * ldexp(T(1), static_cast<int>(i+1) / 2) - 0.5f);
   }
   else
   {
      // even i:
      unsigned n = i / 2;
      T result = factorial<T>(n, pol);
      if(ldexp(tools::max_value<T>(), -(int)n) > result)
         return result * ldexp(T(1), (int)n);
   }
   //
   // If we fall through to here then the result is infinite:
   //
   return policies::raise_overflow_error<T>("autoboost::math::double_factorial<%1%>(unsigned)", 0, pol);
}

template <class T>
inline T double_factorial(unsigned i)
{
   return double_factorial<T>(i, policies::policy<>());
}

namespace detail{

template <class T, class Policy>
T rising_factorial_imp(T x, int n, const Policy& pol)
{
   AUTOBOOST_STATIC_ASSERT(!autoboost::is_integral<T>::value);
   if(x < 0)
   {
      //
      // For x less than zero, we really have a falling
      // factorial, modulo a possible change of sign.
      //
      // Note that the falling factorial isn't defined
      // for negative n, so we'll get rid of that case
      // first:
      //
      bool inv = false;
      if(n < 0)
      {
         x += n;
         n = -n;
         inv = true;
      }
      T result = ((n&1) ? -1 : 1) * falling_factorial(-x, n, pol);
      if(inv)
         result = 1 / result;
      return result;
   }
   if(n == 0)
      return 1;
   if(x == 0)
   {
      if(n < 0)
         return -autoboost::math::tgamma_delta_ratio(x + 1, static_cast<T>(-n), pol);
      else
         return 0;
   }
   if((x < 1) && (x + n < 0))
   {
      T val = autoboost::math::tgamma_delta_ratio(1 - x, static_cast<T>(-n), pol);
      return (n & 1) ? T(-val) : val;
   }
   //
   // We don't optimise this for small n, because
   // tgamma_delta_ratio is alreay optimised for that
   // use case:
   //
   return 1 / autoboost::math::tgamma_delta_ratio(x, static_cast<T>(n), pol);
}

template <class T, class Policy>
inline T falling_factorial_imp(T x, unsigned n, const Policy& pol)
{
   AUTOBOOST_STATIC_ASSERT(!autoboost::is_integral<T>::value);
   AUTOBOOST_MATH_STD_USING // ADL of std names
   if((x == 0) && (n >= 0))
      return 0;
   if(x < 0)
   {
      //
      // For x < 0 we really have a rising factorial
      // modulo a possible change of sign:
      //
      return (n&1 ? -1 : 1) * rising_factorial(-x, n, pol);
   }
   if(n == 0)
      return 1;
   if(x < 0.5f)
   {
      //
      // 1 + x below will throw away digits, so split up calculation:
      //
      if(n > max_factorial<T>::value - 2)
      {
         // If the two end of the range are far apart we have a ratio of two very large
         // numbers, split the calculation up into two blocks:
         T t1 = x * autoboost::math::falling_factorial(x - 1, max_factorial<T>::value - 2);
         T t2 = autoboost::math::falling_factorial(x - max_factorial<T>::value + 1, n - max_factorial<T>::value + 1);
         if(tools::max_value<T>() / fabs(t1) < fabs(t2))
            return autoboost::math::sign(t1) * autoboost::math::sign(t2) * policies::raise_overflow_error<T>("autoboost::math::falling_factorial<%1%>", 0, pol);
         return t1 * t2;
      }
      return x * autoboost::math::falling_factorial(x - 1, n - 1);
   }
   if(x <= n - 1)
   {
      //
      // x+1-n will be negative and tgamma_delta_ratio won't
      // handle it, split the product up into three parts:
      //
      T xp1 = x + 1;
      unsigned n2 = itrunc((T)floor(xp1), pol);
      if(n2 == xp1)
         return 0;
      T result = autoboost::math::tgamma_delta_ratio(xp1, -static_cast<T>(n2), pol);
      x -= n2;
      result *= x;
      ++n2;
      if(n2 < n)
         result *= falling_factorial(x - 1, n - n2, pol);
      return result;
   }
   //
   // Simple case: just the ratio of two
   // (positive argument) gamma functions.
   // Note that we don't optimise this for small n,
   // because tgamma_delta_ratio is alreay optimised
   // for that use case:
   //
   return autoboost::math::tgamma_delta_ratio(x + 1, -static_cast<T>(n), pol);
}

} // namespace detail

template <class RT>
inline typename tools::promote_args<RT>::type
   falling_factorial(RT x, unsigned n)
{
   typedef typename tools::promote_args<RT>::type result_type;
   return detail::falling_factorial_imp(
      static_cast<result_type>(x), n, policies::policy<>());
}

template <class RT, class Policy>
inline typename tools::promote_args<RT>::type
   falling_factorial(RT x, unsigned n, const Policy& pol)
{
   typedef typename tools::promote_args<RT>::type result_type;
   return detail::falling_factorial_imp(
      static_cast<result_type>(x), n, pol);
}

template <class RT>
inline typename tools::promote_args<RT>::type
   rising_factorial(RT x, int n)
{
   typedef typename tools::promote_args<RT>::type result_type;
   return detail::rising_factorial_imp(
      static_cast<result_type>(x), n, policies::policy<>());
}

template <class RT, class Policy>
inline typename tools::promote_args<RT>::type
   rising_factorial(RT x, int n, const Policy& pol)
{
   typedef typename tools::promote_args<RT>::type result_type;
   return detail::rising_factorial_imp(
      static_cast<result_type>(x), n, pol);
}

} // namespace math
} // namespace autoboost

#endif // AUTOBOOST_MATH_SP_FACTORIALS_HPP

