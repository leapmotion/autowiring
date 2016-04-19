
#ifndef AUTOBOOST_MPL_SWITCH_HPP_INCLUDED
#define AUTOBOOST_MPL_SWITCH_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2003-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/find_if.hpp>
#include <autoboost/mpl/deref.hpp>
#include <autoboost/mpl/lambda.hpp>
#include <autoboost/mpl/apply.hpp>
#include <autoboost/mpl/pair.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>

namespace autoboost { namespace mpl {

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(Body)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(T)
    >
struct switch_
{
    typedef typename find_if<
          Body
        , apply1< lambda< first<_1> >, T >
        >::type iter_;

    typedef typename deref<iter_>::type pair_;
    typedef typename lambda< typename second<pair_>::type >::type f_;
    typedef typename apply1<f_,T>::type type;

    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(2,switch_,(Body,T))
};

AUTOBOOST_MPL_AUX_NA_SPEC(2, switch_)

}}

#endif // AUTOBOOST_MPL_SWITCH_HPP_INCLUDED
