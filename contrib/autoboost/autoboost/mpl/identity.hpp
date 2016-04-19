
#ifndef AUTOBOOST_MPL_IDENTITY_HPP_INCLUDED
#define AUTOBOOST_MPL_IDENTITY_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>

namespace autoboost { namespace mpl {

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(T)
    >
struct identity
{
    typedef T type;
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(1, identity, (T))
};

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(T)
    >
struct make_identity
{
    typedef identity<T> type;
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(1, make_identity, (T))
};

AUTOBOOST_MPL_AUX_NA_SPEC_NO_ETI(1, identity)
AUTOBOOST_MPL_AUX_NA_SPEC_NO_ETI(1, make_identity)

}}

#endif // AUTOBOOST_MPL_IDENTITY_HPP_INCLUDED
