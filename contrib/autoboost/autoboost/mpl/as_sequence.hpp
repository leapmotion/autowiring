
#ifndef AUTOBOOST_MPL_AS_SEQUENCE_HPP_INCLUDED
#define AUTOBOOST_MPL_AS_SEQUENCE_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2002-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/is_sequence.hpp>
#include <autoboost/mpl/single_view.hpp>
#include <autoboost/mpl/if.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>

namespace autoboost { namespace mpl {

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(T)
    >
struct as_sequence
    : if_< is_sequence<T>, T, single_view<T> >
{
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(1,as_sequence,(T))
};

AUTOBOOST_MPL_AUX_NA_SPEC_NO_ETI(1, as_sequence)

}}

#endif // AUTOBOOST_MPL_AS_SEQUENCE_HPP_INCLUDED
