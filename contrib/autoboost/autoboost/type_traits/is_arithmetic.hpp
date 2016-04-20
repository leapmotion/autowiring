
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_ARITHMETIC_HPP_INCLUDED
#define AUTOBOOST_TT_IS_ARITHMETIC_HPP_INCLUDED

#if !defined( __CODEGEARC__ )
#include <autoboost/type_traits/is_integral.hpp>
#include <autoboost/type_traits/is_float.hpp>
#include <autoboost/type_traits/detail/ice_or.hpp>
#include <autoboost/config.hpp>
#endif

// should be the last #include
#include <autoboost/type_traits/detail/bool_trait_def.hpp>

namespace autoboost {

#if !defined(__CODEGEARC__)
namespace detail {

template< typename T >
struct is_arithmetic_impl
{
    AUTOBOOST_STATIC_CONSTANT(bool, value =
        (::autoboost::type_traits::ice_or<
            ::autoboost::is_integral<T>::value,
            ::autoboost::is_float<T>::value
        >::value));
};

} // namespace detail
#endif

//* is a type T an arithmetic type described in the standard (3.9.1p8)
#if defined(__CODEGEARC__)
AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_arithmetic,T,__is_arithmetic(T))
#else
AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_arithmetic,T,::autoboost::detail::is_arithmetic_impl<T>::value)
#endif

} // namespace autoboost

#include <autoboost/type_traits/detail/bool_trait_undef.hpp>

#endif // AUTOBOOST_TT_IS_ARITHMETIC_HPP_INCLUDED
