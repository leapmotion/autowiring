
#ifndef AUTOBOOST_MPL_EMPTY_BASE_HPP_INCLUDED
#define AUTOBOOST_MPL_EMPTY_BASE_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/bool.hpp>
#include <autoboost/mpl/aux_/config/msvc.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>

#include <autoboost/type_traits/integral_constant.hpp>
#include <autoboost/type_traits/is_empty.hpp>

namespace autoboost { namespace mpl {

// empty base class, guaranteed to have no members; inheritance from
// 'empty_base' through the 'inherit' metafunction is a no-op - see
// "mpl/inherit.hpp> header for the details
struct empty_base {};

template< typename T >
struct is_empty_base
    : false_
{
#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1300)
    using false_::value;
#endif
};

template<>
struct is_empty_base<empty_base>
    : true_
{
#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1300)
    using true_::value;
#endif
};

}}

namespace autoboost {

template<> struct is_empty< mpl::empty_base >
    : public ::autoboost::integral_constant<bool,true>
{
public:
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(1,is_empty,(mpl::empty_base))
};

}

#endif // AUTOBOOST_MPL_EMPTY_BASE_HPP_INCLUDED
