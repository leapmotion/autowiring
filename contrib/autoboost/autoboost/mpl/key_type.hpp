
#ifndef AUTOBOOST_MPL_KEY_TYPE_HPP_INCLUDED
#define AUTOBOOST_MPL_KEY_TYPE_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2003-2004
// Copyright David Abrahams 2003-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/key_type_fwd.hpp>
#include <autoboost/mpl/sequence_tag.hpp>
#include <autoboost/mpl/apply_wrap.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>

namespace autoboost { namespace mpl {

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(AssociativeSequence)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(T)
    >
struct key_type
    : apply_wrap2<
          key_type_impl< typename sequence_tag<AssociativeSequence>::type >
        , AssociativeSequence, T>
{
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(2,key_type,(AssociativeSequence,T))
};

AUTOBOOST_MPL_AUX_NA_SPEC(2, key_type)

}}

#endif // AUTOBOOST_MPL_KEY_TYPE_HPP_INCLUDED
