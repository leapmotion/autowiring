
#ifndef AUTOBOOST_MPL_AUX_VALUE_WKND_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_VALUE_WKND_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/static_cast.hpp>
#include <autoboost/mpl/aux_/config/integral.hpp>
#include <autoboost/mpl/aux_/config/eti.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

#if defined(AUTOBOOST_MPL_CFG_BCC_INTEGRAL_CONSTANTS) \
    || defined(AUTOBOOST_MPL_CFG_MSVC_60_ETI_BUG)

#   include <autoboost/mpl/int.hpp>

namespace autoboost { namespace mpl { namespace aux {
template< typename C_ > struct value_wknd
    : C_
{
};

#if defined(AUTOBOOST_MPL_CFG_MSVC_60_ETI_BUG)
template<> struct value_wknd<int>
    : int_<1>
{
    using int_<1>::value;
};
#endif
}}}


#if !defined(AUTOBOOST_MPL_CFG_MSVC_60_ETI_BUG)
#   define AUTOBOOST_MPL_AUX_VALUE_WKND(C) \
    ::AUTOBOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::aux::value_wknd< C > \
/**/
#    define AUTOBOOST_MPL_AUX_MSVC_VALUE_WKND(C) AUTOBOOST_MPL_AUX_VALUE_WKND(C)
#else
#   define AUTOBOOST_MPL_AUX_VALUE_WKND(C) C
#   define AUTOBOOST_MPL_AUX_MSVC_VALUE_WKND(C) \
    ::autoboost::mpl::aux::value_wknd< C > \
/**/
#endif

#else // AUTOBOOST_MPL_CFG_BCC_INTEGRAL_CONSTANTS

#   define AUTOBOOST_MPL_AUX_VALUE_WKND(C) C
#   define AUTOBOOST_MPL_AUX_MSVC_VALUE_WKND(C) C

#endif

#if AUTOBOOST_WORKAROUND(__EDG_VERSION__, <= 238)
#   define AUTOBOOST_MPL_AUX_NESTED_VALUE_WKND(T, C) \
    AUTOBOOST_MPL_AUX_STATIC_CAST(T, C::value) \
/**/
#else
#   define AUTOBOOST_MPL_AUX_NESTED_VALUE_WKND(T, C) \
    AUTOBOOST_MPL_AUX_VALUE_WKND(C)::value \
/**/
#endif


namespace autoboost { namespace mpl { namespace aux {

template< typename T > struct value_type_wknd
{
    typedef typename T::value_type type;
};

#if defined(AUTOBOOST_MPL_CFG_MSVC_ETI_BUG)
template<> struct value_type_wknd<int>
{
    typedef int type;
};
#endif

}}}

#endif // AUTOBOOST_MPL_AUX_VALUE_WKND_HPP_INCLUDED
