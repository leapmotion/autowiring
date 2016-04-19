//  (C) Copyright John Maddock 2006.
//  (C) Copyright Johan Rade 2006.
//  (C) Copyright Paul A. Bristow 2011 (added changesign).

//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_MATH_TOOLS_SIGN_HPP
#define AUTOBOOST_MATH_TOOLS_SIGN_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <autoboost/math/tools/config.hpp>
#include <autoboost/math/special_functions/math_fwd.hpp>
#include <autoboost/math/special_functions/detail/fp_traits.hpp>

namespace autoboost{ namespace math{

namespace detail {

  // signbit

#ifdef AUTOBOOST_MATH_USE_STD_FPCLASSIFY
    template<class T>
    inline int signbit_impl(T x, native_tag const&)
    {
        return (std::signbit)(x);
    }
#endif

    // Generic versions first, note that these do not handle
    // signed zero or NaN.

    template<class T>
    inline int signbit_impl(T x, generic_tag<true> const&)
    {
        return x < 0;
    }

    template<class T>
    inline int signbit_impl(T x, generic_tag<false> const&)
    {
        return x < 0;
    }

#if defined(__GNUC__) && (LDBL_MANT_DIG == 106)
    //
    // Special handling for GCC's "double double" type,
    // in this case the sign is the same as the sign we
    // get by casting to double, no overflow/underflow
    // can occur since the exponents are the same magnitude
    // for the two types:
    //
    inline int signbit_impl(long double x, generic_tag<true> const&)
    {
       return (autoboost::math::signbit)(static_cast<double>(x));
    }
    inline int signbit_impl(long double x, generic_tag<false> const&)
    {
       return (autoboost::math::signbit)(static_cast<double>(x));
    }
#endif

    template<class T>
    inline int signbit_impl(T x, ieee_copy_all_bits_tag const&)
    {
        typedef AUTOBOOST_DEDUCED_TYPENAME fp_traits<T>::type traits;

        AUTOBOOST_DEDUCED_TYPENAME traits::bits a;
        traits::get_bits(x,a);
        return a & traits::sign ? 1 : 0;
    }

    template<class T>
    inline int signbit_impl(T x, ieee_copy_leading_bits_tag const&)
    {
        typedef AUTOBOOST_DEDUCED_TYPENAME fp_traits<T>::type traits;

        AUTOBOOST_DEDUCED_TYPENAME traits::bits a;
        traits::get_bits(x,a);

        return a & traits::sign ? 1 : 0;
    }

    // Changesign

    // Generic versions first, note that these do not handle
    // signed zero or NaN.

    template<class T>
    inline T (changesign_impl)(T x, generic_tag<true> const&)
    {
        return -x;
    }

    template<class T>
    inline T (changesign_impl)(T x, generic_tag<false> const&)
    {
        return -x;
    }
#if defined(__GNUC__) && (LDBL_MANT_DIG == 106)
    //
    // Special handling for GCC's "double double" type,
    // in this case we need to change the sign of both
    // components of the "double double":
    //
    inline long double (changesign_impl)(long double x, generic_tag<true> const&)
    {
       double* pd = reinterpret_cast<double*>(&x);
       pd[0] = autoboost::math::changesign(pd[0]);
       pd[1] = autoboost::math::changesign(pd[1]);
       return x;
    }
    inline long double (changesign_impl)(long double x, generic_tag<false> const&)
    {
       double* pd = reinterpret_cast<double*>(&x);
       pd[0] = autoboost::math::changesign(pd[0]);
       pd[1] = autoboost::math::changesign(pd[1]);
       return x;
    }
#endif

    template<class T>
    inline T changesign_impl(T x, ieee_copy_all_bits_tag const&)
    {
        typedef AUTOBOOST_DEDUCED_TYPENAME fp_traits<T>::sign_change_type traits;

        AUTOBOOST_DEDUCED_TYPENAME traits::bits a;
        traits::get_bits(x,a);
        a ^= traits::sign;
        traits::set_bits(x,a);
        return x;
    }

    template<class T>
    inline T (changesign_impl)(T x, ieee_copy_leading_bits_tag const&)
    {
        typedef AUTOBOOST_DEDUCED_TYPENAME fp_traits<T>::sign_change_type traits;

        AUTOBOOST_DEDUCED_TYPENAME traits::bits a;
        traits::get_bits(x,a);
        a ^= traits::sign;
        traits::set_bits(x,a);
        return x;
    }


}   // namespace detail

template<class T> int (signbit)(T x)
{
   typedef typename detail::fp_traits<T>::type traits;
   typedef typename traits::method method;
   // typedef typename autoboost::is_floating_point<T>::type fp_tag;
   typedef typename tools::promote_args_permissive<T>::type result_type;
   return detail::signbit_impl(static_cast<result_type>(x), method());
}

template <class T>
inline int sign AUTOBOOST_NO_MACRO_EXPAND(const T& z)
{
   return (z == 0) ? 0 : (autoboost::math::signbit)(z) ? -1 : 1;
}

template <class T> typename tools::promote_args_permissive<T>::type (changesign)(const T& x)
{ //!< \brief return unchanged binary pattern of x, except for change of sign bit.
   typedef typename detail::fp_traits<T>::sign_change_type traits;
   typedef typename traits::method method;
   // typedef typename autoboost::is_floating_point<T>::type fp_tag;
   typedef typename tools::promote_args_permissive<T>::type result_type;

   return detail::changesign_impl(static_cast<result_type>(x), method());
}

template <class T, class U>
inline typename tools::promote_args_permissive<T, U>::type
   copysign AUTOBOOST_NO_MACRO_EXPAND(const T& x, const U& y)
{
   AUTOBOOST_MATH_STD_USING
   typedef typename tools::promote_args_permissive<T, U>::type result_type;
   return (autoboost::math::signbit)(static_cast<result_type>(x)) != (autoboost::math::signbit)(static_cast<result_type>(y))
      ? (autoboost::math::changesign)(static_cast<result_type>(x)) : static_cast<result_type>(x);
}

} // namespace math
} // namespace autoboost


#endif // AUTOBOOST_MATH_TOOLS_SIGN_HPP


