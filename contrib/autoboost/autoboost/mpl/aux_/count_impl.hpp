
#ifndef AUTOBOOST_MPL_AUX_COUNT_IMPL_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_COUNT_IMPL_HPP_INCLUDED

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

#include <autoboost/mpl/count_fwd.hpp>
#include <autoboost/mpl/count_if.hpp>
#include <autoboost/mpl/same_as.hpp>
#include <autoboost/mpl/aux_/config/static_constant.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>
#include <autoboost/mpl/aux_/traits_lambda_spec.hpp>

namespace autoboost { namespace mpl {

template< typename Tag > struct count_impl
{
    template< typename Sequence, typename T > struct apply
#if AUTOBOOST_WORKAROUND(__BORLANDC__,AUTOBOOST_TESTED_AT(0x561))
    {
        typedef typename count_if< Sequence,same_as<T> >::type type;
        AUTOBOOST_STATIC_CONSTANT(int, value = AUTOBOOST_MPL_AUX_VALUE_WKND(type)::value);
#else
        : count_if< Sequence,same_as<T> >
    {
#endif
    };
};

AUTOBOOST_MPL_ALGORITM_TRAITS_LAMBDA_SPEC(2,count_impl)

}}

#endif // AUTOBOOST_MPL_AUX_COUNT_IMPL_HPP_INCLUDED
