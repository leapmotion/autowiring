//  Copyright John Maddock 2007.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_MATH_MODF_HPP
#define AUTOBOOST_MATH_MODF_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <autoboost/math/special_functions/math_fwd.hpp>
#include <autoboost/math/tools/config.hpp>
#include <autoboost/math/special_functions/trunc.hpp>

namespace autoboost{ namespace math{

template <class T, class Policy>
inline T modf(const T& v, T* ipart, const Policy& pol)
{
   *ipart = trunc(v, pol);
   return v - *ipart;
}
template <class T>
inline T modf(const T& v, T* ipart)
{
   return modf(v, ipart, policies::policy<>());
}

template <class T, class Policy>
inline T modf(const T& v, int* ipart, const Policy& pol)
{
   *ipart = itrunc(v, pol);
   return v - *ipart;
}
template <class T>
inline T modf(const T& v, int* ipart)
{
   return modf(v, ipart, policies::policy<>());
}

template <class T, class Policy>
inline T modf(const T& v, long* ipart, const Policy& pol)
{
   *ipart = ltrunc(v, pol);
   return v - *ipart;
}
template <class T>
inline T modf(const T& v, long* ipart)
{
   return modf(v, ipart, policies::policy<>());
}

#ifdef AUTOBOOST_HAS_LONG_LONG
template <class T, class Policy>
inline T modf(const T& v, autoboost::long_long_type* ipart, const Policy& pol)
{
   *ipart = lltrunc(v, pol);
   return v - *ipart;
}
template <class T>
inline T modf(const T& v, autoboost::long_long_type* ipart)
{
   return modf(v, ipart, policies::policy<>());
}
#endif

}} // namespaces

#endif // AUTOBOOST_MATH_MODF_HPP
