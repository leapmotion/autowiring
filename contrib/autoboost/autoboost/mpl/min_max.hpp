
#ifndef AUTOBOOST_MPL_MIN_MAX_HPP_INCLUDED
#define AUTOBOOST_MPL_MIN_MAX_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2008
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/less.hpp>
#include <autoboost/mpl/if.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>

namespace autoboost { namespace mpl {

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(N1)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(N2)
    >
struct min
    : if_< less<N1,N2>,N1,N2 >
{
};

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(N1)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(N2)
    >
struct max
    : if_< less<N1,N2>,N2,N1 >
{
};

AUTOBOOST_MPL_AUX_NA_SPEC(2, min)
AUTOBOOST_MPL_AUX_NA_SPEC(2, max)

}}

#endif // AUTOBOOST_MPL_MIN_MAX_HPP_INCLUDED
