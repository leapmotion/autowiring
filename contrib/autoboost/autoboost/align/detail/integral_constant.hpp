/*
Copyright 2014-2016 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AUTOBOOST_ALIGN_DETAIL_INTEGRAL_CONSTANT_HPP
#define AUTOBOOST_ALIGN_DETAIL_INTEGRAL_CONSTANT_HPP

#include <autoboost/config.hpp>

#if !defined(AUTOBOOST_NO_CXX11_HDR_TYPE_TRAITS)
#include <type_traits>
#endif

namespace autoboost {
namespace alignment {
namespace detail {

#if !defined(AUTOBOOST_NO_CXX11_HDR_TYPE_TRAITS)
using std::integral_constant;
#else
template<class T, T Value>
struct integral_constant {
    typedef T value_type;
    typedef integral_constant type;

    AUTOBOOST_CONSTEXPR operator value_type() const AUTOBOOST_NOEXCEPT {
        return Value;
    }

    AUTOBOOST_CONSTEXPR value_type operator()() const AUTOBOOST_NOEXCEPT {
        return Value;
    }

    AUTOBOOST_STATIC_CONSTEXPR T value = Value;
};

template<class T, T Value>
AUTOBOOST_CONSTEXPR_OR_CONST T integral_constant<T, Value>::value;
#endif

} /* detail */
} /* alignment */
} /* autoboost */

#endif
