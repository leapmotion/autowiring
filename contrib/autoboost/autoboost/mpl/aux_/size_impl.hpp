
#ifndef AUTOBOOST_MPL_AUX_SIZE_IMPL_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_SIZE_IMPL_HPP_INCLUDED

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

#include <autoboost/mpl/size_fwd.hpp>
#include <autoboost/mpl/begin_end.hpp>
#include <autoboost/mpl/distance.hpp>
#include <autoboost/mpl/aux_/traits_lambda_spec.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

namespace autoboost { namespace mpl {

// default implementation; conrete sequences might override it by
// specializing either the 'size_impl' or the primary 'size' template

template< typename Tag >
struct size_impl
{
    template< typename Sequence > struct apply
#if !AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x561))
        : distance<
              typename begin<Sequence>::type
            , typename end<Sequence>::type
            >
    {
#else
    {
        typedef typename distance<
              typename begin<Sequence>::type
            , typename end<Sequence>::type
            >::type type;
#endif
    };
};

AUTOBOOST_MPL_ALGORITM_TRAITS_LAMBDA_SPEC(1, size_impl)

}}

#endif // AUTOBOOST_MPL_AUX_SIZE_IMPL_HPP_INCLUDED
