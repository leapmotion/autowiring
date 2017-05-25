
#ifndef AUTOBOOST_MPL_INDEX_OF_HPP_INCLUDED
#define AUTOBOOST_MPL_INDEX_OF_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2003-2004
// Copyright Eric Friedman 2003
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/index_if.hpp>
#include <autoboost/mpl/same_as.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>

namespace autoboost { namespace mpl {

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(Sequence)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(T)
    >
struct index_of
    : index_if< Sequence,same_as<T> >
{
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(2,index_of,(Sequence,T))
};

AUTOBOOST_MPL_AUX_NA_SPEC(2, index_of)

}}

#endif // AUTOBOOST_MPL_INDEX_OF_HPP_INCLUDED
