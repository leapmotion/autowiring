
#ifndef AUTOBOOST_MPL_PAIR_HPP_INCLUDED
#define AUTOBOOST_MPL_PAIR_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/msvc_eti_base.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>
#include <autoboost/mpl/aux_/config/eti.hpp>

namespace autoboost { namespace mpl {

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(T1)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(T2)
    >
struct pair
{
    typedef pair type;
    typedef T1 first;
    typedef T2 second;

    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(2,pair,(T1,T2))
};

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(P)
    >
struct first
{
#if !defined(AUTOBOOST_MPL_CFG_MSVC_70_ETI_BUG)
    typedef typename P::first type;
#else
    typedef typename aux::msvc_eti_base<P>::first type;
#endif
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(1,first,(P))
};

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(P)
    >
struct second
{
#if !defined(AUTOBOOST_MPL_CFG_MSVC_70_ETI_BUG)
    typedef typename P::second type;
#else
    typedef typename aux::msvc_eti_base<P>::second type;
#endif
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(1,second,(P))
};


AUTOBOOST_MPL_AUX_NA_SPEC_NO_ETI(2, pair)
AUTOBOOST_MPL_AUX_NA_SPEC(1, first)
AUTOBOOST_MPL_AUX_NA_SPEC(1, second)

}}

#endif // AUTOBOOST_MPL_PAIR_HPP_INCLUDED
