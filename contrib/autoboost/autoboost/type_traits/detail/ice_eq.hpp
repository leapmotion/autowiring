//  (C) Copyright John Maddock and Steve Cleary 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_DETAIL_ICE_EQ_HPP_INCLUDED
#define AUTOBOOST_TT_DETAIL_ICE_EQ_HPP_INCLUDED

#include <autoboost/config.hpp>

namespace autoboost {
namespace type_traits {

template <int b1, int b2>
struct ice_eq
{
    AUTOBOOST_STATIC_CONSTANT(bool, value = (b1 == b2));
};

template <int b1, int b2>
struct ice_ne
{
    AUTOBOOST_STATIC_CONSTANT(bool, value = (b1 != b2));
};

#ifndef AUTOBOOST_NO_INCLASS_MEMBER_INITIALIZATION
template <int b1, int b2> bool const ice_eq<b1,b2>::value;
template <int b1, int b2> bool const ice_ne<b1,b2>::value;
#endif

} // namespace type_traits
} // namespace autoboost

#endif // AUTOBOOST_TT_DETAIL_ICE_EQ_HPP_INCLUDED
