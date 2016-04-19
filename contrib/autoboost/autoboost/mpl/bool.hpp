
#ifndef AUTOBOOST_MPL_BOOL_HPP_INCLUDED
#define AUTOBOOST_MPL_BOOL_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/bool_fwd.hpp>
#include <autoboost/mpl/integral_c_tag.hpp>
#include <autoboost/mpl/aux_/config/static_constant.hpp>

AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_OPEN

template< bool C_ > struct bool_
{
    AUTOBOOST_STATIC_CONSTANT(bool, value = C_);
    typedef integral_c_tag tag;
    typedef bool_ type;
    typedef bool value_type;
    AUTOBOOST_CONSTEXPR operator bool() const { return this->value; }
};

#if !defined(AUTOBOOST_NO_INCLASS_MEMBER_INITIALIZATION)
template< bool C_ >
bool const bool_<C_>::value;
#endif

AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_CLOSE

#endif // AUTOBOOST_MPL_BOOL_HPP_INCLUDED
