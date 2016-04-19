
#ifndef AUTOBOOST_MPL_BACK_HPP_INCLUDED
#define AUTOBOOST_MPL_BACK_HPP_INCLUDED

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

#include <autoboost/mpl/back_fwd.hpp>
#include <autoboost/mpl/aux_/back_impl.hpp>
#include <autoboost/mpl/sequence_tag.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>

namespace autoboost { namespace mpl {

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(Sequence)
    >
struct back
    : back_impl< typename sequence_tag<Sequence>::type >
        ::template apply< Sequence >
{
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(1,back,(Sequence))
};

AUTOBOOST_MPL_AUX_NA_SPEC(1, back)

}}

#endif // AUTOBOOST_MPL_BACK_HPP_INCLUDED
