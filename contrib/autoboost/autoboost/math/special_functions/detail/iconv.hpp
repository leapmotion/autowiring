//  Copyright (c) 2009 John Maddock
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_MATH_ICONV_HPP
#define AUTOBOOST_MATH_ICONV_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <autoboost/math/special_functions/round.hpp>
#include <autoboost/type_traits/is_convertible.hpp>

namespace autoboost { namespace math { namespace detail{

template <class T, class Policy>
inline int iconv_imp(T v, Policy const&, mpl::true_ const&)
{
   return static_cast<int>(v);
}

template <class T, class Policy>
inline int iconv_imp(T v, Policy const& pol, mpl::false_ const&)
{
   AUTOBOOST_MATH_STD_USING
   return iround(v, pol);
}

template <class T, class Policy>
inline int iconv(T v, Policy const& pol)
{
   typedef typename autoboost::is_convertible<T, int>::type tag_type;
   return iconv_imp(v, pol, tag_type());
}


}}} // namespaces

#endif // AUTOBOOST_MATH_ICONV_HPP

