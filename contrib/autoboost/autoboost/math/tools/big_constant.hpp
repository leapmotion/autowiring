
//  Copyright (c) 2011 John Maddock
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_MATH_TOOLS_BIG_CONSTANT_HPP
#define AUTOBOOST_MATH_TOOLS_BIG_CONSTANT_HPP

#include <autoboost/math/tools/config.hpp>
#ifndef AUTOBOOST_MATH_NO_LEXICAL_CAST
#include <autoboost/lexical_cast.hpp>
#endif
#include <autoboost/type_traits/is_convertible.hpp>

namespace autoboost{ namespace math{

namespace tools{

template <class T>
struct numeric_traits : public std::numeric_limits< T > {};

#ifdef AUTOBOOST_MATH_USE_FLOAT128
typedef __float128 largest_float;
#define AUTOBOOST_MATH_LARGEST_FLOAT_C(x) x##Q
template <>
struct numeric_traits<__float128>
{
   static const int digits = 113;
   static const int digits10 = 33;
   static const int max_exponent = 16384;
   static const bool is_specialized = true;
};
#else
typedef long double largest_float;
#define AUTOBOOST_MATH_LARGEST_FLOAT_C(x) x##L
#endif

template <class T>
inline AUTOBOOST_CONSTEXPR_OR_CONST T make_big_value(largest_float v, const char*, mpl::true_ const&, mpl::false_ const&) AUTOBOOST_MATH_NOEXCEPT(T)
{
   return static_cast<T>(v);
}
template <class T>
inline AUTOBOOST_CONSTEXPR_OR_CONST T make_big_value(largest_float v, const char*, mpl::true_ const&, mpl::true_ const&) AUTOBOOST_MATH_NOEXCEPT(T)
{
   return static_cast<T>(v);
}
#ifndef AUTOBOOST_MATH_NO_LEXICAL_CAST
template <class T>
inline T make_big_value(largest_float, const char* s, mpl::false_ const&, mpl::false_ const&)
{
   return autoboost::lexical_cast<T>(s);
}
#endif
template <class T>
inline AUTOBOOST_MATH_CONSTEXPR const char* make_big_value(largest_float, const char* s, mpl::false_ const&, mpl::true_ const&) AUTOBOOST_MATH_NOEXCEPT(T)
{
   return s;
}

//
// For constants which might fit in a long double (if it's big enough):
//
#define AUTOBOOST_MATH_BIG_CONSTANT(T, D, x)\
   autoboost::math::tools::make_big_value<T>(\
      AUTOBOOST_MATH_LARGEST_FLOAT_C(x), \
      AUTOBOOST_STRINGIZE(x), \
      mpl::bool_< (is_convertible<autoboost::math::tools::largest_float, T>::value) && \
      ((D <= autoboost::math::tools::numeric_traits<autoboost::math::tools::largest_float>::digits) \
          || is_floating_point<T>::value \
          || (autoboost::math::tools::numeric_traits<T>::is_specialized && \
          (autoboost::math::tools::numeric_traits<T>::digits10 <= autoboost::math::tools::numeric_traits<autoboost::math::tools::largest_float>::digits10))) >(), \
      autoboost::is_convertible<const char*, T>())
//
// For constants too huge for any conceivable long double (and which generate compiler errors if we try and declare them as such):
//
#define AUTOBOOST_MATH_HUGE_CONSTANT(T, D, x)\
   autoboost::math::tools::make_big_value<T>(0.0L, AUTOBOOST_STRINGIZE(x), \
   mpl::bool_<is_floating_point<T>::value || (autoboost::math::tools::numeric_traits<T>::is_specialized && autoboost::math::tools::numeric_traits<T>::max_exponent <= autoboost::math::tools::numeric_traits<autoboost::math::tools::largest_float>::max_exponent && autoboost::math::tools::numeric_traits<T>::digits <= autoboost::math::tools::numeric_traits<autoboost::math::tools::largest_float>::digits)>(), \
   autoboost::is_convertible<const char*, T>())

}}} // namespaces

#endif

